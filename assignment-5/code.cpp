#include <bits/stdc++.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;
using namespace std;

const int id_max = 5, type_max = 4, N = 15;

int id=0, type = 0;
int h, w;
bool kernel[N][N];

Mat input;

bool check(int x, int y, int n, int m) {
	return x>=0 and y>=0 and x<n and y<m;
}

Mat dilate(Mat input) {

	Mat output = input.clone();

	for(int x=0; x<input.rows; x++) {
		for(int y=0; y<input.cols; y++) {
			int val = 0, curr;
			for(int i=0; i<h; i++) {
				for(int j=0; j<w; j++) {
					int xx = x+(i-h/2);
					int yy = y+(j-w/2);
					if(!check(xx, yy, input.rows, input.cols) or !kernel[i][j])
						continue;
					curr = input.at<uchar>(xx,yy)/255;
					val |= curr;
				}
			}
			val *= 255;
			output.at<uchar>(x,y) = val;
		}
	}

	imshow("Erode", output);

	return output;
}

Mat erode(Mat input) {
	Mat output = input.clone();

	for(int x=0; x<input.rows; x++) {
		for(int y=0; y<input.cols; y++) {
			int val = 1, curr;
			for(int i=0; i<h; i++) {
				for(int j=0; j<w; j++) {
					int xx = x+(i-h/2);
					int yy = y+(j-w/2);
					if(!check(xx, yy, input.rows, input.cols) or !kernel[i][j])
						continue;
					curr = input.at<uchar>(xx,yy)/255;
					val &= curr;
				}
			}
			val *= 255;
			output.at<uchar>(x,y) = val;
		}
	}

	imshow("Dilate", output);

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
			h = 1, w = 3;
			break;
		case 2:
			h = 3, w = 3;
			break;
		case 3:
			h = 3, w = 3;
			kernel[0][0] = kernel[0][2] = kernel[2][0] = kernel[2][2] = 0;
			break;
		case 4:
			h = 9, w = 9;
			break;
		case 5:
			h = 15, w = 15;
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

	return;
}

int main() {
    
	string file;
	cout << "File Name: ";
	//cin >> file;
	file = "ricegrains.bmp";
    input = imread(file, 0);

	namedWindow("Structure Tracker", WINDOW_AUTOSIZE);
    createTrackbar("Structure", "Structure Tracker", &id, id_max, on_change);
    createTrackbar("Transformation", "Structure Tracker", &type, type_max, on_change);
    imshow("Structure Tracker", input);

    waitKey(0);
    return 0;
}