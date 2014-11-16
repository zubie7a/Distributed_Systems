#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <cmath>
#include <ctime>
#include <queue>
// Some standard C/C++ Libraries to deal with data structures, calculations
// measuring time, and some Input / Output operations from plain text files.

using namespace std;
// Namespace for C++ STD

vector<string> getMatrixName();
void validateArguments(int NUM_USERS, int NUM_MOVIES, int NUM_MATCHES);
bool checkArguments(int NUM_USERS, int NUM_MOVIES, int NUM_MATCHES);