#include <opencv2/opencv.hpp>
#include <filesystem>
#include "lib/EqualizeTransform.hpp"
#include "lib/CVcommonTask.hpp"
#include "lib/ImagePath.hpp"

using namespace std;
using namespace cv;
namespace fs = std::filesystem; 

Mat match(Mat input, Mat target) {

  int maxPixelVal = (1<<8) -1;
  EqualizeTransform tr1(input, maxPixelVal), tr2(target, maxPixelVal);

  return applyToEach(input, [&](int val){

    return tr2.invTransform(tr1.transform(val));

  });
}

int main(int argc, char const *argv[])
{ 
  // printf("code begins\n");
  if( argc < 4){
    cout<<"usage: ./histogram_matching <path_to_input_image> <path_to_target_image> <path_to_output_image>\n";
    return -1;
  } else {
    string inputImagePath = argv[1];
    string targetImagePath = argv[2];
    string outputImageDir = argv[3];

    if (!fs::is_directory(outputImageDir) || !fs::exists(outputImageDir)) { 
      fs::create_directory(outputImageDir); 
    }

    Mat inputImage = imread(inputImagePath);

    Mat inuputHistogram = getHistPlot(inputImage);
    imshow("input histogram", inuputHistogram);

    Mat targetImage = imread(targetImagePath);
    ImagePath inputImagePathObj(inputImagePath), targetImagePathObj(targetImagePath);
    string inputImageName = inputImagePathObj.getImageName();
    string imageExtension = inputImagePathObj.getExtension();
    string outputImageName = inputImageName + "matched_to_" + targetImagePathObj.getImageName();

    Mat outputImage = match(inputImage, targetImage);

    Mat outputHistogram = getHistPlot(outputImage);
    imshow("output histogram", outputHistogram);

    imwrite(outputImageDir + outputImageName + imageExtension, outputImage);
    imwrite(outputImageDir + outputImageName + "_histogram.jpg" + imageExtension,outputHistogram);
    imwrite(outputImageDir + inputImageName + "_histogram.jpg",inuputHistogram);
    imwrite(outputImageDir + targetImagePathObj.getImageName() + "_histogram.jpg",getHistPlot(targetImage));
    waitKey(0);
  }
  return 0;
}
