#include <bits/stdc++.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <filesystem>
using namespace cv;
using namespace std;

const int id_max = 5, type_max = 4, N = 15;

int id=0, type = 0;
int h, w;
int centerH, centerW;
int centerH_hat,centerW_hat;
bool kernel[N][N];
bool kernet_hat[N][N];

Mat input;

bool check(int x, int y, int n, int m) {
	return x>=0 and y>=0 and x<n and y<m;
}

Mat dilate(Mat input) {

	Mat output = input.clone();
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			kernet_hat[i][j] = kernel[h-1-i][w-1-j];
		}
		
	}
	centerH_hat = h-1-centerH;
	centerW_hat = w-1-centerW;

	for(int x=0; x<input.rows; x++) {
		for(int y=0; y<input.cols; y++) {
			int val = 0, curr;
			for(int i=0; i<h; i++) {
				for(int j=0; j<w; j++) {
					int xx = x + (i - centerH_hat);
					int yy = y + (j - centerW_hat);
					if(!check(xx, yy, input.rows, input.cols))
						continue;
					curr = (input.at<uchar>(xx,yy)/255) * (int) kernet_hat[i][j];
					val |= curr;
				}
			}
			val *= 255;
			output.at<uchar>(x,y) = val;
		}
	}

	imshow("dilation", output);

	return output;
}

Mat erode(Mat input) {
	Mat output = input.clone();

	for(int x=0; x<input.rows; x++) {
		for(int y=0; y<input.cols; y++) {
			int val = 1, curr;
			for(int i=0; i<h; i++) {
				for(int j=0; j<w; j++) {
					int xx = x + (i - centerH);
					int yy = y + (j - centerW);
					if(!check(xx, yy, input.rows, input.cols)){
						val = 0;
						break;
					}
					curr = (input.at<uchar>(xx,yy)/255)* (int) kernel[i][j];
					val &= curr;
				}
			}
			val *= 255;
			output.at<uchar>(x,y) = val;
		}
	}

	imshow("erosion", output);

	return output;
}

void openclose(Mat input) {
	
	Mat output = erode(dilate(input));
	imshow("OpenClose", output);	

	return;
}

void closeopen(Mat input) {

	Mat output = dilate(erode(input));
	imshow("CloseOpen", output);

	return;
}

static void on_change(int, void*) {

	for(int i=0; i<N; i++)
		for(int j=0; j<N; j++)
			kernel[i][j] = 1;
	
	switch(id) {
		case 1:
			h = 1, w = 2;
			centerH = 0;
			centerW = 0;
			break;
		case 2:
			h = 3, w = 3;
			centerH = 1;
			centerW = 1;
			break;
		case 3:
			h = 3, w = 3;
			centerH = 1;
			centerW = 1;
			kernel[0][0] = kernel[0][2] = kernel[2][0] = kernel[2][2] = 0;
			break;
		case 4:
			h = 9, w = 9;
			centerH = 4;
			centerW = 4;
			break;
		case 5:
			h = 15, w = 15;
			centerH = 14;
			centerW = 14;
			break;
		default:
			return;
	}

	switch(type) {
		case 1:
			dilate(input);
			break;
		case 2:
			erode(input);
			break;
		case 3:
			openclose(input);
			break;
		case 4:
			closeopen(input);
			break;
		default:
			return;
	}
}

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		cout << "usage ./code <path to input image>\n";
		return -1;
	}
	cv::Mat input = cv::imread(argv[1], 0);
	namedWindow("Structure Tracker", WINDOW_AUTOSIZE);
    createTrackbar("Structure", "Structure Tracker", &id, id_max, on_change);
    createTrackbar("Transformation", "Structure Tracker", &type, type_max, on_change);
    imshow("Structure Tracker", input);

    waitKey(0);
    return 0;
}