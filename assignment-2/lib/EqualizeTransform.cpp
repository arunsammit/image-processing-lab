#include "EqualizeTransform.hpp"
#include<algorithm>
EqualizeTransform::EqualizeTransform(Mat input, int maxIntensityVal){
  EqualizeTransform::maxIntensityVal =  maxIntensityVal;
  transformation.resize(maxIntensityVal + 1);
  for (int i = 0; i < input.rows; i++)
  {
    for (int j = 0; j < input.cols; j++)
    {
      Vec3b intensity = input.at<Vec3b>(i,j);
      for (int k = 0; k < input.channels(); k++)
      {
        int currentVal = intensity.val[k];
        if(currentVal <= maxIntensityVal)
          transformation[intensity.val[k]]++;
        else{
          throw std::invalid_argument("Given Mat object contains intensity greater than " + to_string(maxIntensityVal)
            + " at position i = " + to_string(i) + " j = " + to_string(j) + " and k = " + to_string(k));
        }
      }
      
    }
    
  }
  int prev = transformation.front();
  for (int i = 1; i < transformation.size(); i++)
  {
    transformation[i] += prev;
    prev = transformation[i];
  }
  int max_val = transformation.back();
  for (int i = 0; i < transformation.size(); i++)
  {
    transformation[i] = round( (maxIntensityVal * ((double)transformation[i]/max_val )));
  }
  

}
int EqualizeTransform::transform(int val){
  if(val < transformation.size()){
    return transformation[val];
  } else {
    throw std::invalid_argument("val can't be greater than "+ to_string(transformation.size()));
  }
}

int EqualizeTransform::invTransform(int val){
  vector<int>::iterator it = lower_bound(transformation.begin(), transformation.end(), val);
  if (it == transformation.end()) {
    return (transformation.end() - 1) - transformation.begin();
  } else if (it == transformation.begin()) {
    return 0;
  } else {

    if (*it - val < *(it-1) - val) {
      return it - transformation.begin();
    }
    else {
      return (it - 1) - transformation.begin();
    }
    
  }

}