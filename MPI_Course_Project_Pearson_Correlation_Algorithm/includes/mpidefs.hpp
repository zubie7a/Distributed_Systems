#include <mpi.h>
// OpenMPI library to deal with multiprocessor running of this program, under
// the SIMD model (Single Instruction, Multiple Data), as in, the same program
// will be running on separate processors, but each one will process different
// sets of data, to join the results in the end.
// THESE GO IN THE TAG FIELD, VERY IMPORTANT! When a process reaches a receive
// command, it will block, and wait until such command is completed. It will
// specify a buffer, a type of data, a length, a sender, and a tag. The TAG is
// very important because the same sender may send different kinds of data and
// we want to know which kind of data is it.

#define FROM_MASTER 1
// To identify Master sending a Worker the original rating matrix to a Worker.
#define FROM_WORKER 2
// To identify a Worker sending the Master the best ranked Users for a User.
#define FROM_MASTER_P 42
// To identify Master sending a Worker the program's parameters
#define FROM_MASTER_X 70
// Dummy signal/tag to halt Workers at the end to prevent them from finishing
// the MPI while the Master is merging all results.
#define MASTER 0
// The ID of the Master is 0, kind of like a forking program. We check if its
// the Master of a Worker if the TASK_ID is 0 or greater than it.