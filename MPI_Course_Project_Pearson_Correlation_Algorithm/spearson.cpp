#include "./includes/cppdefs.cpp"
#include "./includes/matches.cpp"

int main(int argc, char *argv[]) {
    int NUM_USERS, NUM_MOVIES, NUM_BEST, NUM_PROCS = 1;
    int *matrixUI;
    int *params = new int[3];
    vector<string> matrixName;
    matrixName = getMatrixName();
    if(matrixName.size() < 1) {
        fprintf(stderr, "Input file was not found.\n");
        exit(1);
    }
    string fname = matrixName[0];
    ifstream iss(fname);
    iss >> NUM_USERS >> NUM_MOVIES >> NUM_BEST;
    validateArguments(NUM_USERS, NUM_MOVIES, NUM_BEST);
    matrixUI = new int[NUM_USERS * NUM_MOVIES];
    for(int i = 0; i < NUM_USERS; i++) {
        for(int j = 0; j < NUM_MOVIES; j++) {
            int rating;
            iss >> rating;
            matrixUI[(i * NUM_MOVIES) + j] = rating;
        }
    }    
    vector< int* > results;
    for(int U1 = 0; U1 < NUM_USERS; U1 += NUM_PROCS) {
        int *recList;
        recList = findBests(NUM_USERS, NUM_MOVIES, NUM_BEST, U1, matrixUI);
        results.push_back(recList);
    }
    int **recMat;
    recMat = new int* [NUM_USERS];
    for(int i = 0; i < results.size(); i++) {
        int USER = i * NUM_PROCS;
        int *list = results[i];
        recMat[USER] = list;
    }
    system("mkdir -p ./results");
    ofstream oss("./results/sresult.txt");
    for(int i = 0; i < NUM_USERS; i++){
        for(int j = 0 ; j < NUM_BEST; j++) {
            oss << recMat[i][j] << " ";
        }
        oss << endl;
    }
    delete params;
    delete matrixUI;
    for(int i = 0; i < NUM_USERS; i++) {
        delete recMat[i];
    } 
    delete recMat;
    exit(0);
}