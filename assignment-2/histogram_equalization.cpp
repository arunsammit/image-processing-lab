#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include <filesystem>
#include "lib/EqualizeTransform.hpp"
#include "lib/ImagePath.hpp"
#include "lib/CVcommonTask.hpp"

using namespace std;
using namespace cv;
namespace fs = std::filesystem; 

Mat equalize(Mat input){
  Mat out = input.clone();
  int maxPixelValue = (1<<8) - 1;
  EqualizeTransform et(input,maxPixelValue);
  return applyToEach(input, [&](int val){

    return et.transform(val);

  });
}

int main(int argc, char const *argv[])
{

  String input_image_dir, output_image_dir;
  if(argc != 3){
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
    Mat inputHistogram = getHistPlot(input_image);
    if(input_image.empty()){
      cout<<"can't open the image"<<endl;
      return -1;
    }
    
    ImagePath imPath(entry.path());
    string image_name = imPath.getImageName();
    string image_extension = imPath.getExtension();

    Mat out = equalize(input_image);
    Mat outputHistogram = getHistPlot(out);

    string output_image_name = image_name + "_equalized";
    imshow(output_image_name + "_histogram", outputHistogram);
    imshow(image_name +"_histogram", inputHistogram);
    imwrite(output_image_dir + output_image_name + image_extension, out);
  }
  waitKey(0);
  return 0;
}
