#include "./includes/cppdefs.cpp"
#include "./includes/imgcv.cpp"
#define MASTER 0

int main(int argc, char *argv[]) {
    int TASK_ID = 0;
    int HEIGHT, WIDTH, BSIZE, HBLOCKS, WBLOCKS, TBLOCKS;
    if(!parseArguments(argc, argv, &HEIGHT, &WIDTH, &BSIZE)) {
        exit(1);
    }
    srand(time(NULL));
    HBLOCKS = HEIGHT / BSIZE;
    WBLOCKS = WIDTH / BSIZE;
    TBLOCKS = HBLOCKS * WBLOCKS;
    vector<string> frameNames;
    frameNames = getFrameNames();
    if(frameNames.size() < 2) {
        fprintf(stderr, "Not enough frames were found.\n");
        exit(1);
    }
    vector< int* > macroblocks;
    int *frameA;
    int *frameB;
    frameA = loadFrame(frameNames[0], HEIGHT, WIDTH);
    frameB = loadFrame(frameNames[1], HEIGHT, WIDTH);
    int BCOUNT = 0;
    for(int i = 0; i < HEIGHT; i += BSIZE) {
        for(int j = 0; j < WIDTH; j += BSIZE) {
            int *macroblock;
            macroblock = new int [BSIZE * BSIZE];
            for(int k = 0; k < BSIZE; k++) {
                for(int l = 0; l < BSIZE; l++) {
                    int blockIndex = (k * BSIZE) + l;
                    int frameIndex = ((i + k) * WIDTH) + (j + l);
                    macroblock[blockIndex] = frameA[frameIndex];
                }
            }
            macroblocks.push_back(macroblock);
            BCOUNT++;
        }
    }
    vector< pair<double, double> > MOTIONVECTORS;
    for(int k = 0; k < macroblocks.size(); k++) {
        int BNUM = TASK_ID + (k * 1);
        pair<double, double> MV = findMotionVector(macroblocks[k], frameB,
                                                   BNUM, HBLOCKS, WBLOCKS,
                                                   BSIZE, HEIGHT, WIDTH,
                                                   0);
        MOTIONVECTORS.push_back(MV);
    }
    vector< vector<double> > magnitudes(1);
    vector< vector<double> > angles(1);
    for(int k = 0; k < MOTIONVECTORS.size(); k++) {
        magnitudes[MASTER].push_back(MOTIONVECTORS[k].first);
        angles[MASTER].push_back(MOTIONVECTORS[k].second);
    }
    drawVectors(frameNames[0], magnitudes, angles,
                HEIGHT, WIDTH, BSIZE, WBLOCKS, HBLOCKS, 1);
    delete frameA;
    delete frameB;
    for(int k = 0; k < macroblocks.size(); k++) {
        delete macroblocks[k];
    }
    exit(0);
}