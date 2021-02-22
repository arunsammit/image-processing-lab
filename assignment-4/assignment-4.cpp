#include <opencv2/opencv.hpp>

#include <complex>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <valarray>
#include <vector>

#define N 512
#define pi 3.141592654

using namespace std;
using namespace std::complex_literals;
typedef std::complex<double> comp;

vector<cv::Mat> images;
int imagePos, filterPos, freqPos;

namespace FFT
{
    vector<comp> transform1d(const vector<comp> x)
    {
        int n = x.size();
        if (n == 1)
            return x;
        vector<comp> X(n);
        vector<comp> even, odd;
        for (size_t i = 0; i < n; i++)
        {
            even.push_back(x[2*i]);
            odd.push_back(x[2*i+1]);
        }
        for (size_t k = 0; k < n; k++)
        {
            X[k] = even[(k % (n / 2))] + exp(-2i * (pi * k / n)) * odd[(k % (n / 2))];
        }
        return X;
    }

    vector<comp> iTransform1d(vector<comp> X)
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
        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < N; j++)
            {
                out[i][j] = log( 1 + abs( X[ (i - N/2 + N)%N ][ (j - N/2 + N)%N ] ) );
            }
        }
        return out;
    }


    vector<vector<comp>> transform2d(vector<vector<comp>> x)
    {
        for (size_t i = 0; i < N; i++)
        {
            x[i] = FFT::transform1d(x[i]);
        }
        for (size_t i = 0; i < N; i++)   
        {
            vector<comp> temp(N);
            for (size_t j = 0; j < N; j++)
            {
                temp[j] = x[i][j]; 
            }
            temp = FFT::transform1d(temp);
            for (size_t j = 0; j < N; j++)
            {
                x[i][j] = temp[j];
            }
        }
        return x;
    }
    vector<vector<comp>> transform2d(cv::Mat x){
        return transform2d(FFT::toVector(x));
    }

    cv::Mat inverseTransform2d(vector<vector<comp>> X)
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

    cv::Mat toMat(const vector<vector<comp>> &X)
    {
        auto x = cv::Mat(X.size(), X[0].size(), CV_8UC1);
        double max_val;
        for (size_t i = 0; i < X.size(); i++)
        {
            for (size_t j = 0; j < X[i].size(); j++)
            {
                max_val = max( X[i][j].real(), max_val);
            }
            
        }
        
        for (size_t i = 0; i < X.size(); i++)
        {
            for (size_t j = 0; j < X[i].size(); j++)
            {
                x.at<int>(i, j) = (255 / max_val) * (X[i][j].real());
            }
        }
        return x;
    }

    vector<vector<comp>> toVector(cv::Mat x){
        vector<vector<comp>> res(x.rows, vector<comp>(x.cols));
        for (int i = 0; i < x.rows; i++)
        {
            for (int j = 0; j < x.cols; j++)
            {
                res[i][j] = x.at<int>(i,j);
            }
        }
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
        enum _
        {
            Ideal = 0,
            Gaussian,
            Butterworth,
        };

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
        enum _
        {
            Ideal = 3,
            Gaussian,
            Butterworth,
        };
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

    auto output = cv::Mat();
    vector<vector<comp>> outputFFT;

    switch (filterPos)
    {
    case Filter::LowPass::Ideal:
        outputFFT = Filter::LowPass::ideal(inputFFT, 20 * (freqPos + 1));
        break;
    case Filter::HighPass::Ideal:
        outputFFT = Filter::HighPass::ideal(inputFFT, 20 * (freqPos + 1));
        break;
    case Filter::LowPass::Gaussian:
        outputFFT = Filter::LowPass::gaussian(inputFFT, 20 * (freqPos + 1));
        break;
    case Filter::HighPass::Gaussian:
        outputFFT = Filter::HighPass::gaussian(inputFFT, 20 * (freqPos + 1));
        break;
    case Filter::LowPass::Butterworth:
        outputFFT = Filter::LowPass::butterworth(inputFFT, 20 * (freqPos + 1));
        break;
    case Filter::HighPass::Butterworth:
        outputFFT = Filter::HighPass::butterworth(inputFFT, 20 * (freqPos + 1));
        break;
    }
    cv::vconcat(
        input,
        FFT::toMat(FFT::shift2d(inputFFT)),
        input);
    cv::vconcat(
        FFT::inverseTransform2d(outputFFT),
        FFT::toMat(FFT::shift2d(outputFFT)),
        output);
    cv::hconcat(input, output, display);
    cv::imshow("Frequency Filtering", display);
}

int main()
{
    for (auto &file : std::filesystem::directory_iterator("."))
    {
        auto imagePath = (file.path()).u8string();
        ;
        if (imagePath.find(".jpg") != std::string::npos)
        {
            auto img = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
            if (img.rows == N && img.cols == N)
                images.push_back(img);
        }
    }

    /*for (auto& file : std::filesystem::directory_iterator("./exp_4_images")) {
		cout << file.path() << endl;
		auto imp_path = 
		cout << imp_path.find(".jpg") << endl;

	}*/

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
        9,
        callBack);
    callBack(0, NULL);
    cv::waitKey(0);
    return 0;
}