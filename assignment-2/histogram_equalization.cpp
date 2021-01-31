#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include <filesystem>
#include "EqualizeTransform.hpp"
#include "ImagePath.hpp"

using namespace std;
using namespace cv;
namespace fs = std::filesystem; 

Mat equalize(Mat input){
  Mat out = input.clone();
  int maxPixelValue = (1<<8) - 1;
  EqualizeTransform et(input,maxPixelValue);
  for (int i = 0; i < input.rows; i++)
  {
    for (int  j = 0; j < input.cols; j++)
    {
      Vec3b intensity = input.at<Vec3b>(i,j);
      Vec3b& output_intensity = out.at<Vec3b>(i,j);
      for (int k = 0; k < input.channels(); k++)
      {
        output_intensity.val[k] = et.transform(intensity.val[k]);
      }
      
    }
  }
  return out;
}

int main(int argc, char const *argv[])
{

  String input_image_dir, output_image_dir;
  if(argc < 3){
    cout<<"usage: ./histogram_equalization <input_directory> <output_directory/>\n";
    return -1;
  } else {
    input_image_dir = argv[1];
    output_image_dir = argv[2];
  }


  if (!fs::is_directory(output_image_dir) || !fs::exists(output_image_dir)) { 
    fs::create_directory(output_image_dir); 
  }

  for (const auto & entry : fs::directory_iterator(input_image_dir)){
    Mat input_image = imread(entry.path());
    if(input_image.empty()){
      cout<<"can't open the image"<<endl;
      return -1;
    }
    
    ImagePath imPath(entry.path());
    string image_name = imPath.getImageName();
    string image_extension = imPath.getExtension();

    Mat out = equalize(input_image);
    string output_image_name = image_name + "_equalized";
    imwrite(output_image_dir + output_image_name + image_extension, out);
  }
  return 0;
}
