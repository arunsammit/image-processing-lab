#include<vector>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class EqualizeTransform
{
private:
  int maxIntensityVal;
  vector<int> transformation;
public:
  int transform(int);
  EqualizeTransform(Mat,int);
};
