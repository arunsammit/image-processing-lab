#include <opencv2/opencv.hpp>

#include <complex>
#include <filesystem>
#include <iostream>
#include <vector>

#define N 512
#define pi 3.141592654

using namespace std;
using namespace std::complex_literals;
typedef std::complex<double> comp;

vector<cv::Mat> images;
int imagePos, filterPos, freqPos;

string output_image_file;

namespace FFT
{
    cv::Mat toMat(const vector<vector<comp> > &X)
    {
        cv::Mat output(X.size(), X[0].size(), CV_8UC1);
        double max_val = 1, min_val = 0;
        for (size_t i = 0; i < X.size(); i++)
        {
            for (size_t j = 0; j < X.at(i).size(); j++)
            {
                max_val = max( X.at(i).at(j).real(), max_val);
                min_val = min(X.at(i).at(j).real(), min_val);
            }
            
        }
        
        for (size_t i = 0; i < X.size(); i++)
        {
            for (size_t j = 0; j < X.at(i).size(); j++)
            {
                output.at<uchar>(i, j) =  255 * (X.at(i).at(j).real() - min_val)/(max_val - min_val) ;
            }
        }
        return output;
    }

    vector<vector<comp>> toVector(cv::Mat x){
        vector<vector<comp>> res(x.rows, vector<comp>(x.cols));
        for (int i = 0; i < x.rows; i++)
        {
            for (int j = 0; j < x.cols; j++)
            {
                res.at(i).at(j) = x.at<uchar>(i,j);
            }
        }
        return res;
    }
    vector<comp> transform1d(const vector<comp>& x)
    {
        int n = x.size();
        if (n == 1)
            return x;
        vector<comp> even, odd;
        for (int i = 0; i < n; i++)
        {
            if(i%2 == 1){
                odd.push_back(x.at(i));
            }else{
                even.push_back(x.at(i));
            }
        }
        vector<comp> even_t = transform1d(even);
        vector<comp> odd_t = transform1d(odd);
        vector<comp> X;
        for (size_t k = 0; k < n; k++)
        {
            comp curr = even_t.at((k % (even_t.size()))) + exp(-2i * (pi * k / n)) * odd_t.at((k % (odd_t.size())));
            X.push_back(curr);
        }
        return X;
    }

    vector<comp> iTransform1d(const vector<comp>& X)
    {
        int sz = X.size();
        vector<comp> ip(sz);
        for (size_t i = 0; i < X.size(); i++)
            ip[i] = X[(sz-i)%sz]/((double)sz);
        vector<comp> op = FFT::transform1d(ip);
        return op;
    }

    vector<vector<comp>> shift2d(const vector<vector<comp>> &X)
    {
        int si = X.size(), sj = X[0].size();
        vector<vector<comp>> out(si,vector<comp>(sj));
        for (size_t i = 0; i < si; i++)
        {
            for (size_t j = 0; j < sj; j++)
            {
                out[i][j] = log( 1 + abs( X[ (i - si/2 + si)%si ][ (j - sj/2 + sj)%sj ] ) );
            }
        }
        return out;
    }


    vector<vector<comp>> transform2d(const vector<vector<comp>>& x)
    {
        vector<vector<comp>> X;
        for (size_t i = 0; i < x.size(); i++)
        {
            X.push_back(FFT::transform1d(x.at(i)));
        }
        for (size_t j = 0; j < X.at(0).size(); j++)   
        {
            vector<comp> temp;
            for (size_t i = 0; i < X.size(); i++)
            {
                temp.push_back(X.at(i).at(j)); 
            }
            vector<comp> tempFFT = FFT::transform1d(temp);
            for (size_t i = 0; i < X.size(); i++)
            {
                X.at(i).at(j) = tempFFT.at(i);
            }
        }
        return X;
    }
    vector<vector<comp>> transform2d(const cv::Mat& x){
        return transform2d(FFT::toVector(x));
    }

    cv::Mat iTransform2d(const vector<vector<comp>>& X)
    {
        int si = X.size(),sj = X[0].size(); 
        vector<vector<comp>> ip(si,vector<comp>(sj));
        for (size_t i = 0; i < si; i++)
        {
            for (size_t j = 0; j < sj; j++)
            {
                ip[i][j] = X[ (si-i)%si ][ (sj-j)%sj ] / (double) (si * sj);
            }
            
        }
        vector<vector<comp>> op = FFT::transform2d(ip);
        return FFT::toMat(op);
        
    }

}

namespace Filter
{
    double Distance(int i,int j, int si, int sj){
        return abs(comp((si / 2 + i) % si - si / 2, (sj / 2 + j) % sj - sj / 2));
    }
    vector<vector<comp>> ApplyFilter(const vector<vector<comp>>& X, function<comp(double)> f){
        vector<vector<comp>> out(N, vector<comp>(N, 0));
        int si = X.size(), sj = X[0].size();
        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < N; j++)
            {
                double distance = Filter::Distance(i, j, si, sj);
                out[i][j] = X[i][j] * f(distance);
            }
        }
        return out;
    }
    namespace LowPass
    {

        vector<vector<comp>> ideal(const vector<vector<comp>>& X, double cutoff)
        {
            function<comp(double)> Fideal = [&](double distance) -> comp {
                return (distance<=cutoff)?1:0;
            };
            return Filter::ApplyFilter(X, Fideal);
        }

        vector<vector<comp>> gaussian(const vector<vector<comp>>& X, int sigma)
        {
            function<comp(double)> Fgaussian = [&](double distance) -> comp {
                return exp(-distance * distance / (2 * sigma * sigma));
            };
            return Filter::ApplyFilter(X, Fgaussian);
        }

        vector<vector<comp>> butterworth(const vector<vector<comp>> X, int cutoff, int order = 1)
        {
            function<comp(double)> Fbuttorworth = [&](double distance) -> comp {
                return 1/(1 + pow(distance/cutoff ,2*order));
            };
            return Filter::ApplyFilter(X, Fbuttorworth);
        }
    };
    namespace HighPass
    {
        vector<vector<comp>> ideal(const vector<vector<comp>> &X, double cutoff)
        {
            function<comp(double)> Fideal = [&](double distance) -> comp {
                return (distance > cutoff) ? 1 : 0;
            };
            return Filter::ApplyFilter(X, Fideal);
        }

        vector<vector<comp>> gaussian(const vector<vector<comp>> &X, int sigma)
        {
            function<comp(double)> Fgaussian = [&](double distance) -> comp {
                return 1 - exp(-distance * distance / (2 * sigma * sigma));
            };
            return Filter::ApplyFilter(X, Fgaussian);
        }

        vector<vector<comp>> butterworth(const vector<vector<comp>> X, int cutoff, int order = 1)
        {
            function<comp(double)> Fbuttorworth = [&](double distance) -> comp {
                return 1 / (1 + pow(cutoff / distance, 2 * order));
            };
            return Filter::ApplyFilter(X, Fbuttorworth);
        }
    };
};

static void callBack(int, void *)
{
    cv::Mat display;
    cv::Mat input = images.at(imagePos);
    vector<vector<comp>> inputFFT = FFT::transform2d(input);
    vector<vector<comp>> outputFFT;
    double cutoff_freq = 1 << (freqPos + 3); 
    switch (filterPos)
    {
    case 0:
        outputFFT = Filter::LowPass::ideal(inputFFT, cutoff_freq);
        break;
    case 1:
        outputFFT = Filter::LowPass::gaussian(inputFFT, cutoff_freq);
        break;
    case 2:
        outputFFT = Filter::LowPass::butterworth(inputFFT, cutoff_freq);
        break;
    case 3:
        outputFFT = Filter::HighPass::ideal(inputFFT, cutoff_freq);
        break;
    case 4:
        outputFFT = Filter::HighPass::gaussian(inputFFT, cutoff_freq);
        break;
    case 5:
        outputFFT = Filter::HighPass::butterworth(inputFFT, cutoff_freq);
        break;
    }
    cv::Mat inputFFTMat = FFT::toMat(FFT::shift2d(inputFFT));
    cv::Mat outputFFTMat = FFT::toMat(FFT::shift2d(outputFFT));
    cv::Mat iFFToutputFFT = FFT::iTransform2d(outputFFT);
    cv::Mat ip_side, op_side; 
    cv::vconcat(
        input,
        inputFFTMat,
        ip_side
        );
    cv::vconcat(
        iFFToutputFFT,
        outputFFTMat,
        op_side
        );
    cv::hconcat(ip_side, op_side, display);
    cv::imwrite(output_image_file, display);
    cv::imshow("Frequency Filtering", display);
}

int main(int argc, char const *argv[])
{
    if(argc != 3){
        cout<<"usage <path to input images folder> <path to output imager> \n";
        return -1;
    }
    output_image_file = argv[2];
    for (auto &file : std::filesystem::directory_iterator(argv[1]))
    {
        cv::Mat img = cv::imread(file.path(), 0);
        images.emplace_back(img);
    }
    cv::namedWindow("Frequency Filtering");
    cv::createTrackbar(
        "Image",
        "Frequency Filtering",
        &imagePos,
        images.size() - 1,
        callBack);
    cv::createTrackbar(
        "Filter",
        "Frequency Filtering",
        &filterPos,
        5,
        callBack);
    cv::createTrackbar(
        "Cutoff frequency",
        "Frequency Filtering",
        &freqPos,
        5,
        callBack);
    callBack(0, NULL);
    cv::waitKey(0);
    return 0;
}