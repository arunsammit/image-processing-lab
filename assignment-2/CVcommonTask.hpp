#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

Mat applyToEach(Mat input,function<int(int)> f);