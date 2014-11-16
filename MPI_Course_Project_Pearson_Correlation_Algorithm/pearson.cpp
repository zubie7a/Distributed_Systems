#include "./includes/mpidefs.hpp"
#include "./includes/cppdefs.cpp"
#include "./includes/matches.cpp"

int main(int argc, char *argv[]) {
// This is a program to find the best matches for a given User in a database of
// Users with registered preferences using the pearson-correlation algorithm.
// The MPI strategy will be, send ALL the processes the table with all Users'
// ratings (which is invariant). Since the data each process needs to compute
// is also contained within the invariant data, no further data sending is nee-
// ded. Then, each process will find the correlation values for one User at a
// time against all other Users in the database, then returning a list with
// just the top M correlated Users, saving space by not having to store a huge
// table with the correlation values between all possible pairs of Users.
    double t1 = MPI_Wtime(); // START COUNTING THE TIME
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
    // TASK_ID: The ID of this particular process. Master is always 0.
    // NUM_WORKERS: The amount of Workers not counting the Master.
    if((RC = MPI_Init(&argc, &argv)) != MPI_SUCCESS) {
        fprintf(stderr, "Cannot initialize MPI_Init. Exiting.\n");
        MPI_Abort(MPI_COMM_WORLD, RC);
        exit(1);
    }
    // Init MPI, pass the argc and argv of the original program to all Workers
    // so that they will be initialized the same way as the Master Node.
    RC = MPI_Comm_rank(MPI_COMM_WORLD, &TASK_ID);
    // Get TASK_ID of this process, each process will have a different ID.
    RC = MPI_Comm_size(MPI_COMM_WORLD, &NUM_PROCS);
    // Get the total number of processes, given via shell with -np flag.
    int LEN;
    char NAME[MPI_MAX_PROCESSOR_NAME];
    // To store the current host name.
    MPI_Get_processor_name(NAME, &LEN);
    NUM_WORKERS = NUM_PROCS - 1;
    // The number of Workers is the amount of total processes, minus 1.
    printf("Hello! I'm process %d out of %d processes, on host %s.\n",
           TASK_ID, NUM_PROCS, NAME);
    // DEBUG printing to identify that all processes are up and running.
    int NUM_USERS, NUM_MOVIES, NUM_BEST;
    // NUM_USERS:  Amount of registered Users
    // NUM_MOVIES: Amount of available Movies
    // NUM_BEST: The top M values to select from a Users' correlation list,
    // this means, the top M Users a given User is the most correlated with.
    int *matrixUI;
    // Lets initialize the matrix for User->Movie rating values.
    // matrixUI[UserAmount][MovieAmount]
    // User = Y/i/rows, Movie = X/j/cols
    // matrixUI[i][j] == User i rating over Movie j
    int *params = new int[3];
    // To pass the parameters between Nodes.
    if(TASK_ID == MASTER) {
    // Set-up Code for Master Node
        vector<string> matrixName;
        matrixName = getMatrixName();
        if(matrixName.size() < 1) {
        // Exit the program if no text file was found in the ratings folder.
            fprintf(stderr, "Input file was not found.\n");
            MPI_Abort(MPI_COMM_WORLD, RC);
            exit(1);
        }
        string fname = matrixName[0];
        ifstream iss(fname);
        iss >> NUM_USERS >> NUM_MOVIES >> NUM_BEST;
        // Users: 100
        // Movies: 100
        // Amount of Bests Matches: 5
        params[0] = NUM_USERS;
        params[1] = NUM_MOVIES;
        params[2] = NUM_BEST;
        for(int TARGET_ID = 1; TARGET_ID <= NUM_WORKERS; TARGET_ID++) {
        // Send to each Worker the parameters of the program.
           MPI_Send(&(params[0]), 3, MPI_INT, TARGET_ID,
                    FROM_MASTER_P, MPI_COMM_WORLD);
        }
        validateArguments(NUM_USERS, NUM_MOVIES, NUM_BEST);
        // If the three given parameters are valid, proceed to read the
        // matrix containing the ratings each User has for each Movie.
        matrixUI = new int[NUM_USERS * NUM_MOVIES];
        for(int i = 0; i < NUM_USERS; i++) {
            for(int j = 0; j < NUM_MOVIES; j++) {
                int rating;
                iss >> rating;
                matrixUI[(i * NUM_MOVIES) + j] = rating;
            }
        }
        for(int TARGET_ID = 1; TARGET_ID <= NUM_WORKERS; TARGET_ID++) {
        // Now that the matrix has been read, lets send it to all Workers.
            MPI_Send(&(matrixUI[0]), NUM_USERS * NUM_MOVIES, MPI_INT, TARGET_ID,
                     FROM_MASTER, MPI_COMM_WORLD);
        }
    }
    else {
    // Set-up Code for Worker Nodes
        MPI_Recv(&(params[0]), 3, MPI_INT, MASTER, FROM_MASTER_P,
                 MPI_COMM_WORLD, &status);
        // Worker will receive an array containing the program's parameters.
        NUM_USERS  = params[0];
        NUM_MOVIES = params[1];
        NUM_BEST   = params[2];
        validateArguments(NUM_USERS, NUM_MOVIES, NUM_BEST);
        // If the three given parameters are valid, proceed to receive from
        // the Master the matrix containing the ratings each User has for
        // each Movie.
        matrixUI = new int[NUM_USERS * NUM_MOVIES];
        MPI_Recv(&(matrixUI[0]), NUM_USERS * NUM_MOVIES, MPI_INT, MASTER,
                 FROM_MASTER, MPI_COMM_WORLD, &status);
        // Worker will receive an array containing the User -> Movie ratings.
    }
    // Now both Master and Workers have the initial parameters, and the matrix
    // that contains the rating each registerd User has for each Movie.
    vector< int* > results;
    for(int U1 = TASK_ID; U1 < NUM_USERS; U1 += NUM_PROCS) {
    // This is a code that will run in both Master and Workers.
        int *recList;
        recList = findBests(NUM_USERS, NUM_MOVIES, NUM_BEST, U1, matrixUI);
        // Will get the indices of the top M Users this User correlates the
        // most with depending on the ratings the pairs of Users have.
        results.push_back(recList);
        // Store the result for later merging them all.
    }
    int **recMat;
    if(TASK_ID == MASTER) {
        recMat = new int* [NUM_USERS];
        // The Recommendation Matrix is a two dimensional array containing the
        // indices of the top M sers a given User correlates the most with.
        for(int i = 0; i < results.size(); i++) {
            int USER = i * NUM_PROCS;
            int *list = results[i];
            recMat[USER] = list;
        }
        int REMAINDING = NUM_USERS - results.size();
        // The remainding is all the Users' list that were processed by others.
        int indices[NUM_PROCS];
        // Keep track of how many results have been received from each Worker.
        for(int i = 0; i < NUM_PROCS; i++) {
        // Initialize all the counts as 0.
            indices[i] = 0;
        }
        for(int i = 0; i < REMAINDING; i++) {
        // The Master will receive all that was processed by Worker Nodes.
            int *list;
            list = new int [NUM_BEST];
            // list will contain the M Best Matches for a given User, each
            // Worker Node handling different Users.
            MPI_Recv(&(list[0]), NUM_BEST, MPI_INT, MPI_ANY_SOURCE,
                     FROM_WORKER, MPI_COMM_WORLD, &status);
            // Listen to any incoming request from a Worker.
            int SENDER = status.MPI_SOURCE;
            // Get the ID of the Sender of the Request.
            int index = (indices[SENDER] * NUM_PROCS) + SENDER;
            // Calculate the current result's belonging User index.
            indices[SENDER]++;
            // Increase the count of received results.
            recMat[index] = list;
            // Store the current result at the place it belongs to. */
        }
    }
    else {
        for(int i = 0; i < results.size(); i++) {
            MPI_Send(&(results[i][0]), NUM_BEST, MPI_INT, MASTER,
                     FROM_WORKER, MPI_COMM_WORLD);
            // Send to the Master every result obtained by this Worker.
        }
    }
    // Now, lets make the Workers wait for the Master processing results
    // with the vectors with a dummy Send/Recv, otherwise, while the Mas-
    // ter is processing results, the Workers may end the MPI session and
    // make everything go crazy or have the Master interrupt its work.
    // Even the Master has stuff to do!
    if(TASK_ID == MASTER) {
        system("mkdir -p ./results");
        ofstream oss("./results/result.txt");
        for(int i = 0; i < NUM_USERS; i++){
            for(int j = 0 ; j < NUM_BEST; j++) {
                oss << recMat[i][j] << " ";
            }
            oss << endl;
        }
        int value = 0;
        for(int k = 1; k <= NUM_WORKERS; k++) {
            MPI_Send(&value, 1, MPI_INT, k, FROM_MASTER_X, MPI_COMM_WORLD);
            // As a Master, send to every Worker a dummy value to signal
            // the Master has finished doing something not paralellizable.
        }
    }
    else {
        int value = 0;
        MPI_Recv(&value, 1, MPI_INT, MASTER, FROM_MASTER_X,
                 MPI_COMM_WORLD, &status);
        // As a Worker, wait for a certain dummy value to come.
    }
    // NOW, CLEAN MEMORY!
    delete params;
    delete matrixUI;
    if(TASK_ID == MASTER) {
        for(int i = 0; i < NUM_USERS; i++) {
            delete recMat[i];
        }
        delete recMat;
    }
    printf("TASK_ID: %d, Total Time: %lf\n", TASK_ID, MPI_Wtime() - t1);
    // OUTPUT THE TIME IT TOOK FOR THIS PROCESS
    RC = MPI_Finalize();
    exit(0);
}