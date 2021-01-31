#include "CVcommonTask.hpp"

Mat applyToEach(Mat input,function<int(int)> f){
  
  Mat out = input.clone();
  for (int i = 0; i < input.rows; i++)
  {
    for (int  j = 0; j < input.cols; j++)
    {
      Vec3b intensity = input.at<Vec3b>(i,j);
      Vec3b& output_intensity = out.at<Vec3b>(i,j);
      for (int k = 0; k < input.channels(); k++)
      {
        output_intensity.val[k] = f(intensity.val[k]);
      }
      
    }
  }
  return out;
  
}