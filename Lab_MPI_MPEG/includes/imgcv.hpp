#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
// OpenCV 2.0 libraries to deal exclusively with image reading and pixel checks

using namespace cv;
// Namespace for OpenCV

#ifndef CV_LOAD_IMAGE_COLOR
#define CV_LOAD_IMAGE_COLOR IMREAD_COLOR
// 3.0 changes this constant.
#endif

void displayImage(Mat img);
Mat fitImage(Mat img, int HEIGHT, int WIDTH);
int* loadFrame(string filename, int HEIGHT, int WIDTH);
void arrow(Mat img, double magnitude, double angle, int Y, int X);
pair<double, double> getVector(int Y2, int X2, int Y1, int X1);
pair<double, double> findMotionVector(int *macroblock, int* frameB,
                                      int BNUM, int HBLOCKS, int WBLOCKS,
                                      int BSIZE, int HEIGHT, int WIDTH,
                                      int TASK_ID);
void drawVectors(string frameNameA,
                 vector< vector<double> > magnitudes,
                 vector< vector<double> > angles,
                 int HEIGHT, int WIDTH, int BSIZE,
                 int WBLOCKS, int HBLOCKS, int NUM_PROCS);