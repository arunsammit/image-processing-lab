#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include <filesystem>

using namespace std;
using namespace cv;
namespace fs = std::filesystem; 

void getCDF(vector<int>& cdf, Mat input){
  int sz = cdf.size();
  for (int i = 0; i < input.rows; i++)
  {
    for (int j = 0; j < input.cols; j++)
    {
      Vec3b intensity = input.at<Vec3b>(i,j);
      for (int k = 0; k < input.channels(); k++)
      {
        cdf[intensity.val[k]]++;
      }
      
    }
    
  }
  for (int i = 1; i < sz; i++)
  {
    cdf[i] += cdf[i-1]; 
  }
}

Mat equalize(Mat input){
  Mat out = input.clone();
  long sz = 1<<8;
  long intensity_levels = sz -1;
  vector<int> cdf(sz);
  getCDF(cdf, input);
  long max_val = cdf[sz -1];
  for (int i = 0; i < input.rows; i++)
  {
    for (int  j = 0; j < input.cols; j++)
    {
      Vec3b intensity = input.at<Vec3b>(i,j);
      Vec3b& output_intensity = out.at<Vec3b>(i,j);
      for (int k = 0; k < input.channels(); k++)
      {
        output_intensity.val[k] = round( (intensity_levels * ((double)cdf[intensity.val[k]]/max_val )));
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
    fs::create_directory("src"); 
  }

  for (const auto & entry : fs::directory_iterator(input_image_dir)){
    Mat input_image = imread(entry.path());
    if(input_image.empty()){
      cout<<"can't open the image"<<endl;
      return -1;
    }
    stringstream ss1(entry.path());
    string image_name;

    while(getline(ss1,image_name,'/'));
    String image_extension = image_name.substr(image_name.find('.'));
    image_name = image_name.substr(0,image_name.find('.'));
    Mat out = equalize(input_image);
    String output_image_name = image_name + "_equalized";
    namedWindow(output_image_name);
    imshow(output_image_name,out);
    imwrite(output_image_dir + output_image_name + image_extension, out);
  }

  waitKey(0);
  return 0;
}
