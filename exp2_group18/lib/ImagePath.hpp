#include<string>
#include<iostream>
#include<experimental/filesystem>
using namespace std;

class ImagePath
{
private:
  string imagePath;
  string imageName;
  string imageExtension;
public:
  ImagePath(string imagePath);
  string getImageName();
  string getExtension();
};

ImagePath::ImagePath(string imagePath)
{
  ImagePath::imagePath = imagePath;
  stringstream ss1(imagePath);
  while(getline(ss1,imageName,'/'));
  imageName = imageName.substr(0,imageName.find('.'));
  imageExtension = imagePath.substr(imagePath.find('.'));
}
string ImagePath::getImageName(){
  return imageName;
}
string ImagePath::getExtension(){
  return imageExtension;
}
