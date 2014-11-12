#include "./cppdefs.hpp"

vector<string> getFrameNames() {
// This will look in a folder for frames and get their names so we process the
// two found frames, or the first two in the found lexicographical ordering.
    system("find ./frames -type f -regex '\.*frame-1.png' > ./frame-1.txt");
    system("find ./frames -type f -regex '\.*frame-2.png' > ./frame-2.txt");
    // This will list the currently existing frames and put it into a temporary
    // text file which we'll use as a buffer to get filenames into this program
    ifstream iss1("./frame-1.txt");
    ifstream iss2("./frame-2.txt");
    // ifstream = input file stream, a cool way of reading from a file in C++!
    string s;
    vector<string> frameNames;
    // Vector that will hold all the read frame names.
    while(iss1 >> s) {
        frameNames.push_back(s);
    }
    while(iss2 >> s) {
        frameNames.push_back(s);
    }
    system("rm -f ./frame-1.txt");
    system("rm -f ./frame-2.txt");
    // Get rid of the temporary file.
    return frameNames;
}

bool checkArguments(int argc, int HEIGHT, int WIDTH, int BSIZE) {
// Check that the arguments given to the program are valid. If only one of them
// is not valid the program will stop and arguments will be deemed invalid.
    if(WIDTH < 1) {
    // Negative dimension, or value given isn't parseable into an integer!
        fprintf(stderr,
            "Enter a valid value for width, at least 1px (use 640!).\n");
        return false;
    }
    if(HEIGHT < 1) {
    // Negative dimension, or value given isn't parseable into an integer!
        fprintf(stderr,
            "Enter a valid value for width, at least 1px (use 480!).\n");
        return false;
    }
    if(BSIZE < 1) {
    // Negative dimension, or value given isn't parseable into an integer!
        fprintf(stderr,
            "Enter a valid length for block size, at least 1px (use 16!).\n");
        return false;
    }
    if(BSIZE > WIDTH || BSIZE > HEIGHT) {
    // Search block is bigger than the video frame itself, not reasonable.
        fprintf(stderr,
            "Enter a block size such that it doesn't exceeds dimensions!.\n");
        return false;
    }
    if(WIDTH % BSIZE || HEIGHT % BSIZE) {
    // Search block isn't a divisor of both dimensions of the frame.
        fprintf(stderr,
            "Block size must divide both the width and height.\n");
        return false;
    }
    return true;
}

bool parseArguments(int argc, char *argv[],
                    int* HEIGHT, int* WIDTH, int* BSIZE) {
// Parse into integers the arguments given into the program, will fail if there
// aren't exactly 4 arguments, if any of them fail to parse to an integer, and/
// or if any of them is an integer but with invalid values.
    if(argc != 4) {
        fprintf(stderr, "ERROR: Run with 3 arguments as follows: \n");
        fprintf(stderr, "$ ./mpegenc W H B\n");
        fprintf(stderr, "W: Width\nH: Height\nB: Block Size\n");
        return false;
    }
    *HEIGHT = (int)(strtol(argv[2], NULL, 10));
    *WIDTH  = (int)(strtol(argv[1], NULL, 10));
    *BSIZE  = (int)(strtol(argv[3], NULL, 10));
    // Since arguments are read as character arrays, lets try converting to int
    return checkArguments(argc, *HEIGHT, *WIDTH, *BSIZE);
    // We have to make sure that those converted arguments are actually valid!
}