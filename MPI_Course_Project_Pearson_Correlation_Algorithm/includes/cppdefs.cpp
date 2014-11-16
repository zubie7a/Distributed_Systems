#include "./cppdefs.hpp"

vector<string> getMatrixName() {
// Get the name of the first lexicographical .txt file found in ./ratings
    system("find ./ratings -type f -regex '\.*.txt' > ./ratingsList.txt");
    // This will list the current existing files and put them into a temporary
    // text file which we'll use as a buffer to get filenames into the program
    ifstream ifs("./ratingsList.txt");
    // ifstream = input file stream, a cool way of reading from a file in C++!
    string s;
    vector<string> matrixName;
    // Vector that will hold all the read file names.
    ifs >> s;
    matrixName.push_back(s);
    // Put the read string into the filename vector/list.
    system("rm -f ./ratingsList.txt");
    // Get rid of the temporary file.
    return matrixName;
}

bool checkArguments(int NUM_USERS, int NUM_MOVIES, int NUM_MATCHES) {
// Check that the arguments given to the program are valid. If only one of them
// is not valid the program will stop and arguments will be deemed invalid.
    if(NUM_USERS <= 0) {
    // The amount of Users in the given input is either invalid or negative.
        fprintf(stderr,
            "The amount of Users given in the input file is invalid.");
        return false;
    }
    if(NUM_MOVIES <= 0) {
    // The amount of Movies in the given input is either invalid or negative.
        fprintf(stderr,
            "The amount of Movies given in the input file is invalid.");
        return false;
    }
    if(NUM_MATCHES >= NUM_USERS - 1) {
    // If the amount of matches is greater than the amount of Users, counting
    // each User amongst its possible matches (can't match itself!).
        fprintf(stderr,
            "More matches were asked for than Users found in input file.\n");
        return false;
    }
    return true;
}

void validateArguments(int NUM_USERS, int NUM_MOVIES, int NUM_BEST) {
    if(!checkArguments(NUM_USERS, NUM_MOVIES, NUM_BEST)) {
    // If the parse or validation of any of the arguments fails, then finalize
    // the MPI process and exit the program with 1 (usual failure signal).
        MPI_Abort(MPI_COMM_WORLD, 0);
        exit(1);
    }
}