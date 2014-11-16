#include "./includes/cppdefs.cpp"
#include "./includes/mpidefs.hpp"
#include "./includes/imgcv.cpp"

int main(int argc, char *argv[]) {
    double t1 = MPI_Wtime(); // START COUNTING THE TIME
    // First and foremost, initialize all the processes with MPI!!!
    MPI_Status status;
    // This will allow to identify exactly which Worker sent a response to the
    // Master, while receiving requests arbitrarily (no Worker specified in
    // 'source ID' field). This is so we don't block waiting for a specific
    // Worker to answer, because the Workers may finish their operations fast-
    // er than others, so instead, we listen to whatever Worker answers, and
    // then we see what ID this worker has so we can place its response in
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
    // Init MPI, pass the argc and argv of the original program to all workers
    // so that they will be initialized the same way as the master node.
    RC = MPI_Comm_rank(MPI_COMM_WORLD, &TASK_ID);
    // Get TASK_ID of this process, each process will have a different ID.
    RC = MPI_Comm_size(MPI_COMM_WORLD, &NUM_PROCS);
    // Get the total number of processes, given via shell with -np flag.
    int LEN;
    char NAME[MPI_MAX_PROCESSOR_NAME];
    // To store the current host name.
    MPI_Get_processor_name(NAME, &LEN);
    NUM_WORKERS = NUM_PROCS - 1;
    // The number of workers is the amount of total processes, minus 1.
    printf("Hello! I'm process %d out of %d processes, on host.%s\n",
           TASK_ID, NUM_PROCS, NAME);
    // DEBUG printing to identify that all processes are up and running.
    int HEIGHT, WIDTH, BSIZE, HBLOCKS, WBLOCKS, TBLOCKS;
    // HEIGHT:  The target vertical resolution for all frames
    // WIDTH:   The target horizontal resolution for all frames
    // BSIZE:   The side length of the macroblock to search in frames
    // HBLOCKS: Amount of macroblocks on a vertical axis
    // WBLOCKS: Amount of macroblocks on a horizontal axis
    // TBLOCKS: Total amount of macroblocks in a given frame.
    if(!parseArguments(argc, argv, &HEIGHT, &WIDTH, &BSIZE)) {
    // If the parse or validation of any of the arguments fails, then finalize
    // the MPI process and exit the program with 1 (usual failure signal).
        MPI_Abort(MPI_COMM_WORLD, RC);
        exit(1);
    }
    srand(time(NULL));
    // Initialize the seed for the random values based on system time
    HBLOCKS = HEIGHT / BSIZE;
    WBLOCKS = WIDTH / BSIZE;
    TBLOCKS = HBLOCKS * WBLOCKS;
    // This is the total amount of macroblocks there will be, we need to keep
    // track of this because in the end not all processors will handle the same
    // amount of macroblocks, in case the number of processors doesn't divide
    // the number of macroblocks, and knowing the total amount and what blocks
    // each processor has, it becomes easier to find out in the original frame
    // where did that block went without having to pass coordinates explicitly.
    // Less MPI commands is good for you and me!
    vector<string> frameNames;
    if(TASK_ID == MASTER) {
    // Get the framenames by checking into a determinate folder so we don't
    // have to explicitly give a list of framenames to the program.
        frameNames = getFrameNames();
        if(frameNames.size() < 2) {
            fprintf(stderr, "Not enough frames were found.\n");
            MPI_Abort(MPI_COMM_WORLD, RC);
            exit(1);
        }
    }
    vector< int* > macroblocks;
    // List of macroblocks for this process. Will be stored in the order they
    // appear in the original frame, so its easier to put the result back where
    // it belongs without having to pass coordinates in MPI explicitly.
    int *frameA;
    int *frameB;
    // The two frames to compare, 2D arrays represented as 1D for making MPI
    // easier, since we can pass a pointer to stuff thats all contiguous in
    // memory, to save time and requests, the downside is that for large pro-
    // grams a LOT of contiguous space in memory may be a privileged situati-
    // on, so in that case, store the rows in separate arrays, and send row
    // by row, which is better for memory allocation but bad for send time.
    if(TASK_ID == MASTER) {
    // MASTER NODE CODE
        frameA = loadFrame(frameNames[0], HEIGHT, WIDTH);
        frameB = loadFrame(frameNames[1], HEIGHT, WIDTH);
        // Load into memory the two frames we desire to compare.
        double sendT = MPI_Wtime();
        for(int TARGET_ID = 1; TARGET_ID <= NUM_WORKERS; TARGET_ID++) {
        // Send ALL the contents of the target frame to ALL Workers, since
        // it will be invariant while searching for all the macroblocks.
            MPI_Send(&(frameB[0]), HEIGHT * WIDTH, MPI_INT, TARGET_ID,
                     FROM_MASTER_B, MPI_COMM_WORLD);
        }
        // Now that ALL Workers have a copy of ALL the target frame, proceed
        // to give ALL Workers ONLY the macroblocks they have to deal with.
        // Sending them the whole origin frame is a waste of memory and ti-
        // me, lets save as much as possible!
        int BCOUNT = 0;
        // To keep track of the number of blocks sent. The overall index of
        // a block allows to easily reconstruct its position in the origin-
        // al frame knowing the block's dimensions and the frame dimensions!
        for(int i = 0; i < HEIGHT; i += BSIZE) {
            for(int j = 0; j < WIDTH; j += BSIZE) {
                int CUR_ID = BCOUNT % NUM_PROCS;
                // The current ID, or whom does the current macroblock belo
                // -ng to, determined alternately, like in a chess board.
                int *macroblock;
                // 2D Array to hold the values of each macroblock, repres-
                // ented as a 1D array so all elements are contiguous in
                // memory for MPI purposes.
                macroblock = new int [BSIZE * BSIZE];
                for(int k = 0; k < BSIZE; k++) {
                // Copy from the desired area of the origin frame to
                // the macroblock to search for it later.
                    for(int l = 0; l < BSIZE; l++) {
                        int blockIndex = (k * BSIZE) + l;
                        int frameIndex = ((i + k) * WIDTH) + (j + l);
                        macroblock[blockIndex] = frameA[frameIndex];
                    }
                }
                if(CUR_ID == MASTER) {
                // This is a block that the Master itself will deal with.
                    macroblocks.push_back(macroblock);
                }
                else {
                // This is a block that will be sent to a Worker.
                    MPI_Send(&(macroblock[0]), BSIZE * BSIZE, MPI_INT, CUR_ID,
                             FROM_MASTER_A, MPI_COMM_WORLD);
                }
                BCOUNT++;
            }
        }
        // This whole process of creating the macroblocks, and sending them
        // to the workers they belong to, and sending them the target frame
        // too, is a task that's not parallelizable, but it doesn't matter,
        // since this whole setup isn't very computationally intensive. The
        // heavyweight of this application is finding the Motion Vectors!
        printf("Took %lf to dispatch data from master to nodes.\n", 
               MPI_Wtime() - sendT);
    }
    else if(TASK_ID > MASTER) {
    // WORKER NODE CODE
        frameB = new int [HEIGHT * WIDTH];
        // Initialize the target frame, which will be received in its enti-
        // rety, because is the search area for the macroblocks.
        double recvT = MPI_Wtime();
        MPI_Recv(&(frameB[0]), HEIGHT * WIDTH, MPI_INT, MASTER, FROM_MASTER_B,
                 MPI_COMM_WORLD, &status);
        // Receive the pointer to the frameB.
        for(int k = TASK_ID; k < TBLOCKS; k += NUM_PROCS) {
            int *macroblock;
            macroblock = new int [BSIZE * BSIZE];
            MPI_Recv(&(macroblock[0]), BSIZE * BSIZE, MPI_INT, MASTER,
                     FROM_MASTER_A, MPI_COMM_WORLD, &status);
            // Receive the pointer to each of the macroblocks this process
            // will deal with.
            macroblocks.push_back(macroblock);
        }
        printf("Took %lf for node %d to receive all its data.\n", 
               MPI_Wtime() - recvT, TASK_ID);
    }
    // Now all Nodes (MASTER & WORKERS) have target frames AND the macro-
    // blocks each one of them are concerned with. From here on, each Node
    // will work independently, matching the macroblocks from the origin
    // frame in the target frame. As each process finish its task, it will
    // return the results to the Master so that it can merge the answer!
    // DIVIDE AND CONQUER BABY!
    double totalT = 0.0;
    vector< pair<double, double> > MOTIONVECTORS;
    for(int k = 0; k < macroblocks.size(); k++) {
    // The moment of the truth, lets find a macroblock from the first frame
    // in the second frame! This code runs in both Master and Workers!
        int BNUM = TASK_ID + (k * NUM_PROCS);
        // printf("Task %d: macroblock %d.\n", TASK_ID, BNUM);
        // This will give the current block index over all the blocks. This
        // is useful for later retrieving the block's coordinates. To save
        // MPI requests, we didn't get the block's coordinages, BUT these
        // can be reconstructed under the assumpion that all the blocks of
        // a Node were received in the order they appear in the origin fra-
        // me, and with that and the Node ID we can get the coordinates!
        double blockT = MPI_Wtime();
        pair<double, double> MV = findMotionVector(macroblocks[k], frameB,
                                                   BNUM, HBLOCKS, WBLOCKS,
                                                   BSIZE, HEIGHT, WIDTH,
                                                   TASK_ID);
        MOTIONVECTORS.push_back(MV);
        totalT += (MPI_Wtime() - blockT);
    }
    printf("Average time of node %d to calculate 1 block: %lf.\n",
           TASK_ID, totalT / macroblocks.size());
    printf("Total time of node %d to calculate %d blocks: %lf.\n",
           TASK_ID, macroblocks.size(), totalT);
    if(TASK_ID == MASTER) {
    // Setup MASTER to receive vectors from workers
        vector< vector<double> > magnitudes(NUM_PROCS);
        vector< vector<double> > angles(NUM_PROCS);
        // Vectors to store the final results from everyone, all Workers
        // and the Master.
        for(int k = 0; k < MOTIONVECTORS.size(); k++) {
            magnitudes[MASTER].push_back(MOTIONVECTORS[k].first);
            angles[MASTER].push_back(MOTIONVECTORS[k].second);
        }
        int REMAINDING = TBLOCKS - macroblocks.size();
        // REMAINDING is the amount of blocks that the Master did not pro-
        // cess, so we'll set up the Master to receive that amount of res-
        // ults from the Workers.
        double recvT = MPI_Wtime();
        for(int k = 0; k < REMAINDING; k++) {
        // Receive from the Workers the remainding blocks. It is very pos-
        // sible that not all Nodes dealt with the same amount of blocks
        // in situations like the number of processes not being a divisor
        // of the total amount of blocks to process.
            double magnitude;
            double angle;
            int SENDER;
            // We are listening from any source, not waiting for a specific
            // one, but we can know exactly which source we just received a
            // message from, making it easier to handle stuff as it comes!
            MPI_Recv(&magnitude, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
                     FROM_WORKER_A, MPI_COMM_WORLD, &status);
            SENDER = status.MPI_SOURCE;
            magnitudes[SENDER].push_back(magnitude);
            MPI_Recv(&angle, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
                     FROM_WORKER_B, MPI_COMM_WORLD, &status);
            SENDER = status.MPI_SOURCE;
            angles[SENDER].push_back(angle);
        }
        printf("Took %lf for master to receive all nodes' results.\n", 
               MPI_Wtime() - recvT);
        double drawT = MPI_Wtime();
        drawVectors(frameNames[0], magnitudes, angles,
                    HEIGHT, WIDTH, BSIZE, WBLOCKS, HBLOCKS, NUM_PROCS);
        // Just for fun, the motion vectors are going to be drawn in frames!
        printf("Took %lf for master to generate the output image.\n",
               MPI_Wtime() - drawT);
    }
    else if (TASK_ID > MASTER) {
    // Send to Master the list of Motion Vectors this Worker found!
        double sendT = MPI_Wtime();
        for(int k = 0; k < MOTIONVECTORS.size(); k++) {
            double magnitude = MOTIONVECTORS[k].first;
            double angle = MOTIONVECTORS[k].second;
            MPI_Send(&magnitude, 1, MPI_DOUBLE, MASTER, FROM_WORKER_A,
                     MPI_COMM_WORLD);
            MPI_Send(&angle, 1, MPI_DOUBLE, MASTER, FROM_WORKER_B,
                     MPI_COMM_WORLD);
        }
        printf("Took %lf to dispatch results from node %d to master.\n", 
               MPI_Wtime() - sendT, TASK_ID);
    }
    // Now, lets make the Workers wait for the Master creating the image
    // with the vectors with a dummy Send/Recv, otherwise, while the Mas-
    // ter is creating that image, the Workers may end the MPI session and
    // make everything go crazy or have the Master interrupt its work.
    // Even the Master has stuff to do!
    if(TASK_ID == MASTER) {
        int value = 0;
        for(int k = 1; k <= NUM_WORKERS; k++) {
            MPI_Send(&value, 1, MPI_INT, k, FROM_MASTER_A, MPI_COMM_WORLD);
            // As a Master, send to every Worker a dummy value to signal
            // the Master has finished doing something not paralellizable.
        }
    }
    else {
        int value = 0;
        MPI_Recv(&value, 1, MPI_INT, MASTER, FROM_MASTER_A, 
                 MPI_COMM_WORLD, &status);
        // As a Worker, wait for a certain dummy value to come.
    }
    // Now, for some cleanup lets go around freeing some space in the HEAP.
    if(TASK_ID == MASTER) {
    // Only the Master will have the contents of the origin frame.
        delete frameA;
        // Free the pointers to the rows of the origin frame.
    }
    delete frameB;
    // This is needed because we are allocating a lot of space into the
    // HEAP and we don't want any nasty memory leaks to happen anywhere!
    for(int k = 0; k < macroblocks.size(); k++) {
    // Now lets free the memory allocated for all the macroblocks used!
        delete macroblocks[k];
    }
    printf("TASK_ID: %d, Total Time: %lf\n", TASK_ID, MPI_Wtime() - t1);
    // OUTPUT THE TIME IT TOOK FOR THIS PROCESS
    RC = MPI_Finalize();
    exit(0);
}
/*
  Ideally, this is a program that reads frames to calculate the Motion Vectors
  between each pair of consecutive frames, which is the basis for the MPEG enc
  oding. This is done by checking some 'blocks' on the first frame and seeing
  in the next frame where did that block most likely 'moved' (some are comple-
  tely destroyed, some are completely new, some move and remain intact, and so
  me move while losing part of their info). The corresponding place where the
  block moved is the block in the next frame with the least 'difference' with
  the previous one. Then we store a Motion Vector for each block detailing the
  distance and angle of movement.
  ...
  Huge blocks result in less space used (less vectors to store), but LOTS of
  time to calculate them, and in the end, lots of artifacts (since blocks too
  big are more noticeable to the human eye). Small blocks (down to pixel size)
  result in a LOT of space used (a lot of vectors to store), but it takes less
  time to calculate them, and very good quality (fine blocks are unnoticeable)
  showing the inverse relation between compression level and size/quality and
  the direct relation between compression level and the time it takes.
  ...
  These vectors are very useful for predicting/reconstructing images, so less
  frames have to be actually stored, and the missing frames are reconstructed
  by very 'small' Motion Vectors previously calculated (thats why encoding a
  video takes SO LONG and its so COMPUTATIONALLY INTENSIVE). This program will
  deal only with finding these vectors (and painting them over their frames),
  but not with the computing that is done afterwards using these vectors and
  very fancy error correction algorithms to actually encode a video and pred-
  ict images using only these vectors.
  ...
  $ mpirun -np X mpegenc [...]
  X : amount of processes
  [...] : list arguments for the program
*/