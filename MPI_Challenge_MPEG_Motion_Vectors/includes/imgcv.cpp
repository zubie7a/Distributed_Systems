#include "./imgcv.hpp"

void displayImage(Mat img) {
// To display an image in a certain window.
    //img = imread(name, CV_LOAD_IMAGE_COLOR);
    namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Display window", img);
}

Mat fitImage(Mat img, int HEIGHT, int WIDTH) {
// Makes img containing the frame fit the target resolution. This is so the
// target resolution can be manually set, and every frame has that same size.
    Size targetSize(WIDTH, HEIGHT);
    // Size initialization: FIRST WIDTH, THEN HEIGHT
    // Mat  initialization: FIRST HEIGHT, THEN WIDTH
    // Kinda confusing order of dimensions, shouldn't it be standard?...
    if(targetSize != img.size()) {
    // Resize only if the target size is different, or its a computing waste.
        resize(img, img, targetSize);
        // src, dst, siz
        // .. src: Source Image (to take the data from)
        // .. dst: Target Image (to put new data into)
        // .. siz: The Dimensions to apply to the image
        // resize(img, img, Size(), fx, fy);
        // src, dst, Size(), fx, fy
        // .. src: Source Image (to take the data from)
        // .. dst: Target Image (to put new data into)
        // .. Size(): empty Size constructor
        // .. fx: scaling factor in x
        // .. fy: scaling factor in y
    }
    return img;
}

int* loadFrame(string filename, int HEIGHT, int WIDTH) {
// This will read the n-th video frame from several images numbered within 1~n.
// The images read will have been either generated at the generateFrames func-
// tion, or they will be actual images or stills (from a video or something).
    int *frame = new int [HEIGHT * WIDTH];
    Mat img = imread(filename, CV_LOAD_IMAGE_COLOR);
    // Open the desired image.
    img = fitImage(img, HEIGHT, WIDTH);
    // Stretch read image so that all frames have a standardized size.
    int cn = img.channels();
    // Get the amount of channels the image has
    for(int i = 0; i < HEIGHT; i++) {
    // From the obtained images, convert each pixel to HEX.
        for(int j = 0; j < WIDTH; j++) {
            int bgr[3];
            bgr[0] = img.data[((i * WIDTH) + j) * cn + 0]; // B Channel
            bgr[1] = img.data[((i * WIDTH) + j) * cn + 1]; // G Channel
            bgr[2] = img.data[((i * WIDTH) + j) * cn + 2]; // R Channel
            int hexval = 0;
            // Now, start masking all the channels into a single value.
            hexval |= bgr[0] << 16;
            hexval |= bgr[1] <<  8;
            hexval |= bgr[2] <<  0;
            frame[(i * WIDTH) + j] = hexval;
        }
    }
    return frame;
}

pair<double, double> getVector(int Y2, int X2, int Y1, int X1) {
// Based on a pair of ending coordinates and a pair of starting coordinates
// we're going to get the corresponding vector, with magnitude and angle.
    double DY = Y2 - Y1;
    double DX = X2 - X1;
    double angle;
    if(DY == 0) {
        if(DX >= 0) {
            angle = 0.0;
        }
        else {
            angle = 180.0;
        }
        angle = 0.0;
    }
    else if(DX == 0) {
        if(DY < 0) {
            angle = 270.0;
        }
        else {
            angle = 90.0;
        }
    }
    else {
        angle = (atan(DY / DX) * 180.0) / (4.0 * atan(1.0));
        if(DX < 0) {
            angle += 180.0;
        }
        else {
            angle += 360.0;
            angle = fmod(angle, 360.0);
        }
    }
    double magnitude = sqrt((DY * DY) + (DX * DX));
    return make_pair(magnitude, angle);
}

pair<double, double> findMotionVector(int *macroblock, int* frameB,
                                      int BNUM, int HBLOCKS, int WBLOCKS,
                                      int BSIZE, int HEIGHT, int WIDTH,
                                      int TASK_ID) {
// The heuristic to use to search for similar~identical blocks will be a
// BFS, since we want these to be as close as possible, not a row-by-row
// scan that may give a positive result way too far away but there being
// also an useful result closer, which is more 'contiguous motion' like.
// We'll define a tolerance because we feel like its useless to check too
// far away, there's a narrow area of movement in an image.
    bool visited[HEIGHT][WIDTH];
    // A matrix containing 'visited' values. This is because we're going to
    // treat the target frame as a graph, and we'll do a BFS over it to find
    // the blocks that have the 'minimun' change and are the closest possible.
    for(int i = 0; i < HEIGHT; i++) {
    // Clean up, because really the upper declaration will only be done once
    // I think because its a fixed dimension (const)? array. If I don't clean,
    // in other iterations some of these will show up as true, and thats BAD.
        for(int j = 0; j < WIDTH; j++) {
            visited[i][j] = false;
        }
    }
    int BY, BX;
    BY = (BNUM / WBLOCKS) * BSIZE;
    BX = (BNUM % WBLOCKS) * BSIZE;
    // BY and BX are the reconstructed coordinates of the block in the original
    // frame, from BNUM which is this blocks's number amongst all blocks, then
    // WBLOCKS, which is the number of blocks in each row, and BSIZE, the side
    // length of every macroblock.
    // printf("TASK_ID: %d, at %d %d\n", TASK_ID, BY, BX);
    double diff = (double)(1 << 27);
    // Initially the difference will be very big so that we go on storing the
    // positions that give the macroblock the least difference.
    pair<int, int> target(-1,-1);
    queue< pair<int, int> > q;
    q.push(make_pair(BY, BX));
    visited[BY][BX] = true;
    // Push the starting position into the queue to start the BFS.
    int cnt = 0;
    while(q.size()) {
    // BFS will run while there are elements on the queue.
        pair<int, int> p;
        p = q.front();
        q.pop();
        if((p.first + 1) <= (HEIGHT - BSIZE)) {
            if(!visited[p.first + 1][p.second]) {
                q.push(make_pair(p.first + 1, p.second));
                visited[p.first + 1][p.second] = true;
            }
        }
        if((p.first - 1) >= 0) {
            if(!visited[p.first - 1][p.second]) {
                q.push(make_pair(p.first - 1, p.second));
                visited[p.first - 1][p.second] = true;
            }
        }
        if((p.second + 1) <= (WIDTH - BSIZE)) {
            if(!visited[p.first][p.second + 1]) {
                q.push(make_pair(p.first, p.second + 1));
                visited[p.first][p.second + 1] = true;
            }
        }
        if((p.second - 1) >= 0) {
            if(!visited[p.first][p.second - 1]) {
                q.push(make_pair(p.first, p.second - 1));
                visited[p.first][p.second - 1] = true;
            }
        }
        double total = 0;
        for(int i = 0; i < BSIZE; i++) {
            for(int j = 0; j < BSIZE; j++) {
                int hexval1 = macroblock[(i * BSIZE) + j];
                int hexval2 = frameB[((p.first + i) * WIDTH) + (p.second + j)];
                int bgr1[3];
                bgr1[0] = (hexval1 >> 16)       ;
                bgr1[1] = (hexval1 >>  8) & 0xFF;
                bgr1[2] = (hexval1 >>  0) & 0xFF;
                int bgr2[3];
                bgr2[0] = (hexval2 >> 16)       ;
                bgr2[1] = (hexval2 >>  8) & 0xFF;
                bgr2[2] = (hexval2 >>  0) & 0xFF;
                double dB = bgr1[0] - bgr2[0];
                double dG = bgr1[1] - bgr2[1];
                double dR = bgr1[2] - bgr2[2];
                double diff = sqrt((dB * dB) + (dG * dG) + dR * dR);
                total += diff;
            }
        }
        if(total < diff) {
            diff = total;
            target.first  = p.first;
            target.second = p.second;
        }
        if(diff == 0) {
        // Identical match, stop checking against all other possible places.
            break;
        }
        double DY = BY - p.first;
        double DX = BX - p.second;
        double radius = 2 * BSIZE;
        if(sqrt(DY * DY + DX * DX) > radius) {
        // We've gone too far away from the origin, we can disregard this now.
        // There may be coincidental exact matches way too far away from the
        // origin which are not very motion like, so stop.
            break;
        }
    }
    if(target.first == -1 && target.second == -1) {
    // If no target was found, assume it didn't move.
        target.first = BY;
        target.second = BX;
    }
    pair<double, double> MV = getVector(target.first, target.second, BY, BX);
    // Get the Motion Vector using the end coordinates (target) and the origin
    // coordinates (BY, BX, corner of the macroblock were evaluating).
    return MV;
}

void arrow(Mat img, double magnitude, double angle, int Y1, int X1){
// To draw arrows in a frame showing where a macroblock moved!
    int thickness = 1;
    int lineType = 8;
    Point start(X1, Y1);
    // Points use X, Y coordinates, like Size (WIDTH, HEIGHT), strangely
    // only the images seem to have these inverted Mat (HEIGHT, WIDTH) so
    // be careful when dealing with these coordinate swaps.
    double Y2, X2;
    Y2 = magnitude * sin(angle * 4.0 * atan(1.0) / 180.0);
    X2 = magnitude * cos(angle * 4.0 * atan(1.0) / 180.0);
    Point end(X2 + X1, Y2 + Y1);
    line(img, start, end, Scalar(255, 255, 255), thickness, lineType);
    circle(img, Point(end.x, end.y), 2,
           Scalar(255, 255, 255), CV_FILLED, 8, 0);
}

void drawVectors(string frameNameA,
                 vector< vector<double> > magnitudes,
                 vector< vector<double> > angles,
                 int HEIGHT, int WIDTH, int BSIZE,
                 int WBLOCKS, int HBLOCKS, int NUM_PROCS) {
// To draw Motion Vectors over a origin frame and output it!
    Mat final(HEIGHT, WIDTH, CV_8UC3);
    Mat frame1 = imread(frameNameA, CV_LOAD_IMAGE_COLOR);
    frame1 = fitImage(frame1, HEIGHT, WIDTH);
    frame1.copyTo(final);
    for(int TASK_ID = 0; TASK_ID < NUM_PROCS; TASK_ID++) {
    // There will be a TASK_ID from 0 to the number of available processes
        for(int k = 0; k < magnitudes[TASK_ID].size(); k++) {
        // Each of the available processes will have a certain amount of
        // magnitudes and angles (not necessarily the same between them).
            int BNUM = TASK_ID + (k * NUM_PROCS);
            int BY, BX;
            BY = (BNUM / WBLOCKS) * BSIZE + (BSIZE / 2);
            BX = (BNUM % WBLOCKS) * BSIZE + (BSIZE / 2);
            arrow(final, magnitudes[TASK_ID][k], angles[TASK_ID][k], BY, BX);
        }
    }
    system("mkdir -p ./result");
    string filename = "./result/result.png";
    imwrite(filename, final);
}