#include <mpi.h>
// OpenMPI library to deal with multiprocessor running of this program, under
// the SIMD model (Single Instruction, Multiple Data), as in, the same program
// will be running on separate processors, but each one will process different
// sets of data, to join the results in the end.
// THESE GO IN THE TAG FIELD, VERY IMPORTANT! When a process reaches a receive
// command, it will block, and wait until such command is completed. It will
// specify a buffer, a type of data, a length, a sender, and a tag. The TAG is
// very important because the same sender may send different kinds of data and
// we want to know which kind of data is it. In this case, we have two kinds of
// data Workers can receive from a Master, which are values from frameA (the or
// -igin frame) and values from frameB (the target frame).

#define FROM_MASTER_A 1
// To identify Master sending a frameA ~ origin frame value.
#define FROM_MASTER_B 2
// To identify Master sending a frameB ~ target frame value.
#define FROM_WORKER_A 11
// To identify a Worker sending the Magnitude of a vector.
#define FROM_WORKER_B 12
// To identify a Worker sending the Angle of a vector.
#define MASTER 0
// The ID of the Master is 0, kind of like a forking program. We check if its
// the Master of a Worker if the TASK_ID is 0 or greater than it.