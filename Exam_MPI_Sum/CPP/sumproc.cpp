#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <vector>
#include <mpi.h>
#define MASTER 0
// The default value for the Master node
#define FROM_MASTER_L 10
// TAG to identify the Master sending the total amount of lines
#define FROM_MASTER 11
// TAG to identify Master sending data to the Workers
#define FROM_WORKER 12
// TAG to identify Workers sending results to the Master

using namespace std;

int main(int argc, char *argv[]) {
// Note: this program's usage of iostream and fstream and sstream methods of
// input reading and formatting will be significantly slower than the methods
// provided by the teacher, which were C methods for formatting, reading, etc.
    // First and foremost, initialize all the processes with MPI!!!
    MPI_Status status;
    // This will allow to identify exactly which Worker sent a response to the
    // Master, while receiving requests arbitrarily (no Worker specified in
    // 'source ID' field). This is so we don't block waiting for a specific
    // Worker to answer, because the Workers may finish their operations fast-
    // er than others, so instead, we listen to whatever Worker answers, and
    // then we see what ID this Worker has so we can place its response in
    // the place where it belongs, to start merging all the responses.
    int RC, NUM_PROCS, TASK_ID, NUM_WORKERS;
    // RC: The resulting message from several MPI commands to check for errors.
    // NUM_PROCS: The total number of processes counting Workers and Master.
    // TASK_ID: The ID of this particular process. Master is always
    // NUM_WORKERS: The amount of Workers not counting the Master.
    if((RC = MPI_Init(&argc, &argv)) != MPI_SUCCESS) {
        fprintf(stderr, "Cannot initialize MPI_Init. Exiting.\n");
        MPI_Abort(MPI_COMM_WORLD, RC);
        exit(1);
    }
    // Init MPI, pass the argc and argv of the original program to all Workers
    // so that they will be initialized the same way as the Master node.
    RC = MPI_Comm_rank(MPI_COMM_WORLD, &TASK_ID);
    // Get TASK_ID of this process, each process will have a different ID.
    RC = MPI_Comm_size(MPI_COMM_WORLD, &NUM_PROCS);
    // Get the total number of processes, given via shell with -np flag.
    if(TASK_ID == MASTER && argc > 1) {
    // If there's at least one argument (besides the program name), try reading
    // it into an integer, to call the gendata program, otherwise, try reading
    // a text file from the local folder.
        stringstream ss(argv[1]);
        int val = 0;
        ss >> val;
        if(val == 0) {
            fprintf(stderr, "Can't read value, 100 will be used by default.\n");
            val = 100;
        }
        stringstream itos;
        itos << val;
        string command = "./gendata " + itos.str();
        system(command.c_str());
    }
    double t1 = MPI_Wtime(); // START COUNTING THE TIME
    // Now call the program that will generate the randomized input.
    int LEN;
    char NAME[MPI_MAX_PROCESSOR_NAME];
    // To store the current host name.
    MPI_Get_processor_name(NAME, &LEN);
    NUM_WORKERS = NUM_PROCS - 1;
    // The number of Workers is the amount of total processes, minus 1.
    printf("Hello! I'm process %d out of %d processes, on host %s.\n",
           TASK_ID, NUM_PROCS, NAME);
    // DEBUG printing to identify that all processes are up and running.
    int lineNum;
    vector< char* > stringList;
    if(TASK_ID == MASTER) {
    // Code for the Master node
        ifstream ifs("data_in.txt");
        string line;
        vector< vector< char * > > stringLists(NUM_PROCS);
        // First dimension: amount of processes
        // Second dimension: amount of strings for each process to get
        lineNum = 0;
        while(getline(ifs, line)) {
        // Read from the input file stream
            int CUR_PROC = lineNum % NUM_PROCS;
            // Current process to assign the currently read line.
            char *str = new char[42];
            strcpy(str, line.c_str());
            // Convert the C++ string to the traditional C format.
            stringLists[CUR_PROC].push_back(str);
            // Give that process the currently read line
            lineNum++;
            // Increase the amount of lines read until reaching the total
        }
        for(int TARGET_ID = 1; TARGET_ID <= NUM_WORKERS; TARGET_ID++) {
        // Send all the Workers the total amount of lines read
            MPI_Send(&lineNum, 1, MPI_INT, TARGET_ID, FROM_MASTER_L,
                     MPI_COMM_WORLD);
        }
        int iter = 0;
        while(iter < (lineNum / NUM_PROCS) + 1) {
        // This is so we can print in order the results from each node after
        // all of them have been received, printing first the first results,
        // then the second results, and so on.
            for(int CUR_ID = 0; CUR_ID < NUM_PROCS; CUR_ID++) {
                if(iter < stringLists[CUR_ID].size()) {
                    char *str = stringLists[CUR_ID][iter];
                    if(CUR_ID == 0) {
                        stringList.push_back(str);
                    }
                    else {
                        MPI_Send(str, 42, MPI::CHAR, CUR_ID, FROM_MASTER,
                                 MPI_COMM_WORLD);
                    }
                }
            }
            iter++;
        }
    }
    else if(TASK_ID > MASTER) {
    // Code for the Worker nodes
        MPI_Recv(&lineNum, 1, MPI_INT, MASTER, FROM_MASTER_L,
                 MPI_COMM_WORLD, &status);
        // Receive the total amount of lines. It will be helpful to determine
        // when to stop receiving strings from the Master.
        for(int i = TASK_ID; i < lineNum; i += NUM_PROCS) {
        // Starting at TASK_ID, going up to the limit amount of strings, and
        // increasing by the number of Processes, is a deterministic way to
        // know when to stop listening for MPI requests, as not every process
        // may receive the same amount of strings to process.
            char *str = new char [42];
            MPI_Recv(str, 42, MPI::CHAR, MASTER, FROM_MASTER,
                     MPI_COMM_WORLD, &status);
            stringList.push_back(str);
        }
    }
    for(int i = 0; i < stringList.size(); i++) {
    // Now this is the shared part of the code, it will be run by both the
    // Master and the Worker nodes. Each node has a local list of strings to
    // process which is then modified with the values and returned at the end.
        char *str = stringList[i];
        string result;
        stringstream ss(str);
        double res, val1, val2;
        string op;
        ss >> val1 >> op >> val2;
        if(op == "+") {
            res = val1 + val2;
        }
        else if (op == "-") {
            res = val1 - val2;
        }
        else if (op == "*") {
            res = val1 * val2;
        }
        else if (op == "/") {
            res = val1 / val2;
        }
        stringstream os;
        os << val1 << " " << op << " " << val2 << " = " << res;
        // Format back the resulting string.
        result = os.str();
        strcpy(stringList[i], result.c_str());
    }
    // Now, after all nodes have finished processing their data, it comes
    // the final part of the program, which is to merge / output results.
    if(TASK_ID == MASTER) {
    // Code for the Master node.
        vector< vector< char* > > results(NUM_PROCS);
        // Place to merge all the results.
        for(int i = 0; i < stringList.size(); i++) {
        // Put the Master Node's own results there first.
            results[0].push_back(stringList[i]);
        }
        int REMAINING = lineNum - stringList.size();
        // Find out the remainding amount of results to wait for.
        for(int i = 0; i < REMAINING; i++) {
            char *str = new char [42];
            MPI_Recv(str, 42, MPI::CHAR, MPI_ANY_SOURCE, FROM_WORKER,
                     MPI_COMM_WORLD, &status);
            // Receive several results from any source, checking afterwards
            // the value of the origin node.
            int SENDER = status.MPI_SOURCE; 
            results[SENDER].push_back(str);
            // Store the result at the respective location of the matrix.
        }
        printf("TASK_ID: %d, Total Time: %lf\n", TASK_ID, MPI_Wtime() - t1);
        int iter = 0;
        ofstream ofs("./data_out.txt");
        while(iter < (lineNum / NUM_PROCS) + 1) {
        // This is so we can print in order the results from each node after
        // all of them have been received, printing first the first results,
        // then the second results, and so on.
            for(int i = 0; i < NUM_PROCS; i++) {
                if(iter < results[i].size()) {
                    ofs << results[i][iter] << endl;
                }
            }
            iter++;
        }
    }
    else if (TASK_ID > MASTER) {
        for(int i = 0; i < stringList.size(); i++) {
            MPI_Send(stringList[i], 42, MPI::CHAR, MASTER, FROM_WORKER,
                     MPI_COMM_WORLD);
        }
        printf("TASK_ID: %d, Total Time: %lf\n", TASK_ID, MPI_Wtime() - t1);
    }
    // The Master will take longer while generating the output file.
    RC = MPI_Finalize();
    exit(0);
}