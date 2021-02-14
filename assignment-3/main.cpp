#include <bits/stdc++.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;
using namespace std;

const int file_max = 16, width_max = 9, filter_max = 9;
int file_id = 0, width = 1, filter = 2;

String directory[17]; 


Mat input;

// bool check(int x, int y, int n, int m) {
// 	if(x-width/2>=0 and x+width/2<n and y-width/2>=0 and y+width/2<m)
// 		return 1;
// 	return 0;
// }

// int truncate(int x) {
// 	if(x<0)
// 		return 0;
// 	if(x>255)
// 		return 255;
// 	return x;
// } 

Mat mean_filter() {
	Mat outputr = input.clone();
	blur(input, outputr, Size(width, width));
	imshow("OUTPUTr", outputr);

	double kernel[width_max][width_max], size = width*width, value;
	for(int i=0; i<width; i++)
		for(int j=0; j<width; j++)
			kernel[i][j] = 1/size;

	Mat output = input.clone();

	for(int i=0; i<input.rows; i++) {
		for(int j=0; j<input.cols; j++) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
		    	continue;
			}
		    value = 0;
			for(int x=0; x<width; x++) {
				for(int y=0; y<width; y++) {
					value += (double)input.at<uchar>(i+x-width/2, j+y-width/2)*kernel[x][y];
				}
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
		}
	}
	return output;
}

Mat median_filter() {
	Mat outputr = input.clone();
	medianBlur(input, outputr, width);
	imshow("OUTPUTr", outputr);

	Mat output = input.clone();

	int size = width*width;
	vector<int> arr (size); 

	for(int i=0; i<input.rows; i++) {
		for(int j=0; j<input.cols; j++) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
		    	continue;
			}
			for(int x=0, k=0; x<width; x++) {
				for(int y=0; y<width; y++) {
					arr[k++] = input.at<uchar>(i+x-width/2, j+y-width/2);
				}
			}
			
			sort(arr.begin(), arr.end());
			output.at<uchar>(i, j) = arr[size/2];
		}
	}
	return output;
}

Mat prewitt_filter() {
	int kernelH[width_max][width_max], kernelV[width_max][width_max], size = width*width, value1, value2;
	for(int i=0, cnt1=-1; i<width; i++) {
        if(-cnt1==width/2+1)
            cnt1 = 0;
        for(int j=0, cnt2 = -1; j<width; j++) {
            if(-cnt2==width/2+1)
                cnt2 = 0;
            kernelV[i][j] = cnt1;
            kernelH[i][j] = cnt2;
            if(!cnt2)
                cnt2 = width/2;
            else
                cnt2--;
        }
        if(!cnt1)
            cnt1 = width/2;
        else
            cnt1--;
    }

	Mat output = input.clone();

	for(int i=0; i<input.rows; i++) {
		for(int j=0; j<input.cols; j++) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
		    	continue;
			}
		    value1 = value2 = 0;
			for(int x=0; x<width; x++) {
				for(int y=0; y<width; y++) {
					value1 += input.at<uchar>(i+x-width/2, j+y-width/2)*kernelH[x][y];
					value2 += input.at<uchar>(i+x-width/2, j+y-width/2)*kernelV[x][y];
				}
			}
			int value = sqrt(value1*value1+value2*value2);
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
		}
	}
	return output;
}

Mat laplacian_filter() {
	Mat outputr = input.clone();
	Laplacian(input, outputr, CV_8U, width);
	imshow("OUTPUTr", outputr);

	int kernel[width_max][width_max], size = width*width, value;
	memset(kernel, -1, sizeof(kernel));
	kernel[width/2][width/2] = size-1;

	Mat output = input.clone();

	for(int i=0; i<input.rows; i++) {
		for(int j=0; j<input.cols; j++) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
		    	continue;
			}
		    value = 0;
			for(int x=0; x<width; x++) {
				for(int y=0; y<width; y++) {
					value += input.at<uchar>(i+x-width/2, j+y-width/2)*kernel[x][y];
				}
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
		}
	}
	return output;
}

Mat gussian_filter() {
	Mat outputr = input.clone();
	GaussianBlur(input, outputr, Size(width,width), 1);
	imshow("OUTPUTr", outputr);
	
	double kernel[width_max][width_max], k = 1/sqrt(M_PI*2.0), r, sum = 0, value;
    for(int i=0; i<width; i++) {
        for(int j=0; j<width; j++) {
            r = (i-width/2)*(i-width/2)+(j-width/2)*(j-width/2);
            kernel[i][j] = k*exp(-r/2);
            sum += kernel[i][j];
        }
    }

    Mat output = input.clone();

	for(int i=0; i<input.rows; i++) {
		for(int j=0; j<input.cols; j++) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
		    	continue;
			}
		    value = 0;
			for(int x=0; x<width; x++) {
				for(int y=0; y<width; y++) {
					value += (double)input.at<uchar>(i+x-width/2, j+y-width/2)*kernel[x][y];
				}
			}
			value /= sum;
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
		}
	}
	return output;
}

Mat LoG_filter() {
	return input;
}

static void on_file_change(int, void*);
static void on_width_change(int, void*);
static void on_filter_change(int, void*);


static void on_file_change(int, void*) {
	input = imread(directory[file_id], 0);
	imshow("Tracker", input);
	//imshow("OUTPUT", input);
	on_filter_change(filter, 0);
}

static void on_width_change(int, void*) {
	if(!(width&1))
		width++;
	on_filter_change(filter, 0);
}

static void on_filter_change(int, void*) {
	Mat output;
	if(filter == 1)
	{
		output = mean_filter();
	}
	else if(filter == 2)
	{
		output = median_filter();
	}
	else if(filter == 3)
	{
		output = prewitt_filter();
	}
	else if(filter == 4)
	{
		output = laplacian_filter();
	}
	else if(filter == 5)
	{
		output = gussian_filter();
	}
	else if(filter == 6)
	{
		output = LoG_filter();
	}
	else
	{
		output = input;
	}
	


	// switch(filter) {
	// 	case 1:
	// 		output = mean_filter();
	// 		break;
	// 	case 2:
	// 		output = median_filter();
	// 		break;
	// 	case 3:
	// 		output = prewitt_filter();
	// 		break;
	// 	case 4:
	// 		output = laplacian_filter();
	// 		break;
	// 	case 5:
	// 		output = gussian_filter();
	// 		break;
	// 	case 6:
	// 		output = LoG_filter();
	// 	default:
	// 		output = input;
	// }
	imshow("OUTPUT", output);
}

int main() {
	directory[0] = "Noisy_Images/Cameraman_Salt_Pepper_0_02.jpg";
	directory[1] = "Noisy_Images/Cameraman_Salt_Pepper_0_005.jpg";
	directory[2] = "Noisy_Images/Cameraman_Salt_Pepper_0_08.jpg";
	directory[3] = "Noisy_Images/Camerman_Gaussian_0.005.jpg";
	directory[4] = "Noisy_Images/Camerman_Gaussian_0_05.jpg";
	directory[5] = "Noisy_Images/Pepper_Gaussian_0_01.jpg";
	directory[6] = "Noisy_Images/Pepper_Gaussian_0_005.jpg";
	directory[7] = "Noisy_Images/Pepper_Salt_Pepper_0_02.jpg";
	directory[8] = "Noisy_Images/Pepper_Salt_Pepper_0_005.jpg";
	directory[9] = "Noisy_Images/Pepper_Salt_Pepper_0_08.jpg";
	directory[10] = "Normal_Images/jetplane.jpg";
	directory[11] = "Normal_Images/lake.jpg";
	directory[12] = "Normal_Images/lena_gray_512.jpg";
	directory[13] = "Normal_Images/livingroom.jpg";
	directory[14] = "Normal_Images/mandril_gray.jpg";
	directory[15] = "Normal_Images/pirate.jpg";
	directory[16] = "Normal_Images/walkbridge.jpg";
    
	namedWindow("Tracker", WINDOW_AUTOSIZE);
    createTrackbar("File Name", "Tracker", &file_id, file_max, on_file_change);
    createTrackbar("Kernel width", "Tracker", &width, width_max, on_width_change);
    createTrackbar("Filter Type", "Tracker", &filter, filter_max, on_filter_change); 
    on_file_change(0, 0);

    waitKey(0);
    return 0;
}