#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <string>
#include <cmath>
#include <ctime>
#include <queue>
// Some standard C/C++ Libraries to deal with data structures, calculations
// measuring time, and some Input / Output operations from plain text files.

using namespace std;
// Namespace for C++ STD

vector<string> getFrameNames();
bool checkArguments(int argc, int HEIGHT, int WIDTH, int BSIZE);
bool parseArguments(int argc, char *argv[],
                    int* HEIGHT, int* WIDTH, int* BSIZE);