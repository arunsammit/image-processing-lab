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

auto images = std::vector<cv::Mat>();
int imagePos, filterPos, freqPos;

namespace FFT
{
    vector<vector<comp>> &transpose(vector<vector<comp>> &X)
    {
        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < i; j++)
                swap(X[i][j], X[j][i]);
        }
        return X;
    }
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
        vector<comp> op = FFT::transform1d(X);
        return op;
    }

    vector<vector<double>> shift2d(const vector<vector<comp>> &X)
    {
        vector<vector<double>> out(N,vector<double>(N));
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
        X = FFT::transform2d(X);
        return FFT::toMat(X);
        
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
    namespace LowPass
    {

        valarray<valarray<comp>> ideal(const valarray<valarray<comp>> X, int cutoff)
        {
            auto ret = valarray<valarray<comp>>(valarray<comp>(N), N);
            for (auto i = 0; i != N; ++i)
            {
                for (auto j = 0; j != N; ++j)
                {
                    if (abs(comp((N / 2 + i) % N - N / 2, (N / 2 + j) % N - N / 2)) > cutoff)
                    {
                        ret[i][j] = 0;
                    }
                    else
                    {
                        ret[i][j] = X[i][j];
                    }
                }
            }
            return ret;
        }

        valarray<valarray<comp>> gaussian(const valarray<valarray<comp>> X, int stdDev)
        {
            auto ret = valarray<valarray<comp>>(valarray<comp>(N), N);
            for (auto i = 0; i != N; ++i)
            {
                for (auto j = 0; j != N; ++j)
                {
                    ret[i][j] = X[i][j] *
                                exp(-(pow((N / 2 + i) % N - N / 2, 2) + pow((N / 2 + j) % N - N / 2, 2)) / (2 * pow(stdDev, 2)));
                }
            }
            return ret;
        }

        valarray<valarray<comp>> butterworth(const valarray<valarray<comp>> X, int cutoff, int order = 1)
        {
            auto ret = valarray<valarray<comp>>(valarray<comp>(N), N);
            for (auto i = 0; i != N; ++i)
            {
                for (auto j = 0; j != N; ++j)
                {
                    ret[i][j] = X[i][j] /
                                (1 + pow(abs(comp((N / 2 + i) % N - N / 2, (N / 2 + j) % N - N / 2)) / cutoff, 2 * order));
                }
            }
            return ret;
        }
    };
    namespace HighPass
    {

        valarray<valarray<comp>> ideal(const valarray<valarray<comp>> X, int cutoff)
        {
            auto ret = valarray<valarray<comp>>(valarray<comp>(N), N);
            for (auto i = 0; i != N; ++i)
            {
                for (auto j = 0; j != N; ++j)
                {
                    if (abs(comp((N / 2 + i) % N - N / 2, (N / 2 + j) % N - N / 2)) > cutoff)
                    {
                        ret[i][j] = X[i][j];
                    }
                    else
                    {
                        ret[i][j] = 0;
                    }
                }
            }
            return ret;
        }

        valarray<valarray<comp>> gaussian(const valarray<valarray<comp>> X, int stdDev)
        {
            auto ret = valarray<valarray<comp>>(valarray<comp>(N), N);
            for (auto i = 0; i != N; ++i)
            {
                for (auto j = 0; j != N; ++j)
                {
                    ret[i][j] = X[i][j] *
                                (1 - exp(-(pow((N / 2 + i) % N - N / 2, 2) + pow((N / 2 + j) % N - N / 2, 2)) / (2 * pow(stdDev, 2))));
                }
            }
            return ret;
        }

        valarray<valarray<comp>> butterworth(const valarray<valarray<comp>> X, int cutoff, int order = 1)
        {
            auto ret = valarray<valarray<comp>>(valarray<comp>(N), N);
            for (auto i = 0; i != N; ++i)
            {
                for (auto j = 0; j != N; ++j)
                {
                    ret[i][j] = X[i][j] /
                                (1 + pow(cutoff / abs(comp((N / 2 + i) % N - N / 2, (N / 2 + j) % N - N / 2)), 2 * order));
                }
            }
            return ret;
        }
    };
};

static void callBack(int, void *)
{
    auto display = cv::Mat();
    auto input = images.at(imagePos);
    auto inputFFT = FFT::transform2d(input);

    auto output = cv::Mat();
    auto outputFFT = valarray<valarray<comp>>();

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