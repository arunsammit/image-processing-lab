#include <bits/stdc++.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;
using namespace std;

const int file_max = 16, width_max = 9, filter_max = 9;
int file_id = 0, width = 1, filter = 2;

std::string directory[17]; 

Mat input;


Mat filt_mean() {

	double kernel[width_max][width_max];
	double size = width*width;
	double value;
	int i=0;
	int j=0;
	while(i < width)
	{
		j=0;
		while(j < width)
		{
			kernel[i][j] = 1/size;
			j++;
		}
		i++;
	}

	Mat output = input.clone();
	i=0;j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = (double)input.at<uchar>(i+x-width/2, j+y-width/2) * kernel[x][y];
					value += tmp;
					y++;
				}
				x++;
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}

Mat filt_median() {

	Mat output = input.clone();

	int size = width*width;
	vector<int> arr (width*width); 
	int i=0;
	int j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
			int x=0;
			int y=0;
			int k=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int xx = i+x-width/2;
					int yy = j+y-width/2;
					arr[k++] = input.at<uchar>(xx, yy);
					y++;
				}
				x++;
			}
			
			sort(arr.begin(), arr.end());
			output.at<uchar>(i, j) = arr[size/2];
			j++;
		}
		i++;
	}
	return output;
}

Mat filt_prewitt() {
	int kernelH[width_max][width_max];
	int kernelV[width_max][width_max];
	int size = width*width;
	int value1, value2;
	int i=0;
	int j=0;
	int cnt1=-1;
	int cnt2=-1;
	while(i < width) {
		j=0;
		cnt2=-1;
        if(-cnt1==width/2+1)
            cnt1 = 0;
        while(j < width) {
            if(-cnt2==width/2+1)
                cnt2 = 0;
			kernelH[i][j] = cnt2;
            kernelV[i][j] = cnt1;
            
            if(cnt2)
                cnt2--;
            else
                cnt2 = width/2;
			j++;
        }
        if(cnt1)
            cnt1--;
        else
            cnt1 = width/2;
		i++;
    }

	Mat output = input.clone();
	i=0;j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value1 = value2 = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = input.at<uchar>(i+x-width/2, j+y-width/2) * kernelH[x][y];
					value1 += tmp;
					tmp = input.at<uchar>(i+x-width/2, j+y-width/2) * kernelV[x][y];
					value2 += tmp;
					y++;
				}
				x++;
			}
			int value = sqrt(value1*value1+value2*value2);
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}
Mat sobelH() {
	
	double sigma = 0.75;
	double k = 1/(sqrt(2.0*M_PI)*sigma);
	double r;
	double p[width_max];
	double q[width_max];
	double minv = 100;
	
	
	int i=0;
	int j=0;
	while(i<width) {
		r = (i-width/2)*(i-width/2);
		p[i] = k;
		p[i] *= exp(-r/(2*sigma*sigma));
		minv = min(minv, p[i]);
		if(i<width/2)
			q[i] = -1;
		else if(i>width/2)
			q[i] = 1;
		else
			q[i] = 0;
		i++;
	}
	i=0;
	while(i < width)
	{
		p[i] = (int)(p[i]/minv);
		i++;
	}

	int kernel[width_max][width_max], value;
	i=0;j=0;
    while(i < width) {
		j=0;
        while(j < width) {
            kernel[i][j] = p[i]*q[j];
			j++;
        }
		i++;
    }

    Mat output = input.clone();
	i=0;j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = input.at<uchar>(i+x-width/2, j+y-width/2)*kernel[x][y];
					value += tmp;
					y++;
				}
				x++;
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}

Mat sobelV() {
	double sigma = 0.75;
	double k = 1/(sqrt(2.0*M_PI)*sigma);
	double r;
	double p[width_max];
	double q[width_max];
	double minv = 100;
	
	
	int i=0;
	int j=0;
	while(i<width) {
		r = (i-width/2)*(i-width/2);
		p[i] = k;
		p[i] *= exp(-r/(2*sigma*sigma));
		minv = min(minv, p[i]);
		if(i<width/2)
			q[i] = -1;
		else if(i>width/2)
			q[i] = 1;
		else
			q[i] = 0;
		i++;
	}
	i=0;
	while(i < width)
	{
		p[i] = (int)(p[i]/minv);
		i++;
	}

	int kernel[width_max][width_max], value;
	i=0;j=0;
    while(i < width) {
		j=0;
        while(j < width) {
            kernel[i][j] = p[j]*q[i];
			j++;
        }
		i++;
    }

    Mat output = input.clone();
	i=0;j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = input.at<uchar>(i+x-width/2, j+y-width/2)*kernel[x][y];
					value += tmp;
					y++;
				}
				x++;
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}

Mat filt_lap() {
	int value;
	int size = width*width;
	int kernel[width_max][width_max];
	
	Mat output = input.clone();
	memset(kernel, -1, sizeof(kernel));
	kernel[width/2][width/2] = size-1;

	

	int i=0;
	int j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = input.at<uchar>(i+x-width/2, j+y-width/2)*kernel[x][y];
					value += tmp;
					y++;
				}
				x++;
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}

Mat filt_gaussian() {
	
	double kernel[width_max][width_max];
	double k = 1/sqrt(M_PI*2.0);
	double r;
	double sum = 0;
	double value;
	int i=0;
	int j=0;
    while(i < width) {
		j=0;
        while(j < width) {
            r = (i-width/2)*(i-width/2);
			r += (j-width/2)*(j-width/2);
            kernel[i][j] = k;
			kernel[i][j] *= exp(-r/2);
            sum += kernel[i][j];
			j++;
        }
		i++;
    }

    Mat output = input.clone();
	i=0;j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = (double)input.at<uchar>(i+x-width/2, j+y-width/2)*kernel[x][y];
					value += tmp;
					y++;
				}
				x++;
			}
			value /= sum;
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}

Mat sobelD(){
	int kernel[3][3] = {{0,1,2},{-1,0,-1},{-2,-1,0}};
	double size = 3*3;
	double value;
	int i=0;
	int j=0;
	int width = 3;

	Mat output = input.clone();
	i=0;j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = (double)input.at<uchar>(i+x-width/2, j+y-width/2) * kernel[x][y];
					value += tmp;
					y++;
				}
				x++;
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}

Mat filt_LoG() {
	int kernel[7][7] = {
					{-2, -3, -4, -6, -4, -3, -2},
					{-3, -5, -4, -3, -4, -5, -3},
					{-4, -4,  9, 20,  9, -4, -4},
					{-6, -3, 20, 36, 20, -3, -6},
					{-4, -4,  9, 20,  9, -4, -4},
					{-3, -5, -4, -3, -4, -5, -3},
					{-2, -3, -4, -6, -4, -3, -2}
						};
	double size = 7*7;
	double value;
	int i=0;
	int j=0;
	int width = 7;

	Mat output = input.clone();
	i=0;j=0;
	while(i < input.rows) {
		j=0;
		while(j < input.cols) {
			if(!(i-width/2>=0 and i+width/2<input.rows and j-width/2>=0 and j+width/2<input.cols)) {
				output.at<uchar>(i, j) = 0;
				j++;
		    	continue;
			}
		    value = 0;
			int x=0;
			int y=0;
			while(x < width) {
				y=0;
				while(y < width) {
					int tmp = (double)input.at<uchar>(i+x-width/2, j+y-width/2) * kernel[x][y];
					value += tmp;
					y++;
				}
				x++;
			}
			output.at<uchar>(i, j) = (value > 255) ? 255 : ((value<0) ? 0 : value);
			j++;
		}
		i++;
	}
	return output;
}


static void on_filter_change(int, void*) {
	Mat output;
	if(filter == 1)
	{
		output = filt_mean();
	}
	else if(filter == 2)
	{
		output = filt_median();
	}
	else if(filter == 3)
	{
		output = filt_prewitt();
	}
	else if(filter == 4)
	{
		output = filt_lap();
	}
	else if(filter == 5)
	{
		output = filt_gaussian();
	}
	else if(filter == 6)
	{
		output = filt_LoG();
	}
	else if(filter == 7)
	{
		output = sobelH();
	}
	else if(filter == 8)
	{
		output = sobelV();
	}
	else if(filter == 9)
	{
		output = sobelD();
	}
	else
	{
		output = input;
	}
	imshow("OUTPUT", output);
}

static void on_file_change(int, void*) {
	input = imread(directory[file_id], 0);
	imshow("Tracker", input);
	//imshow("OUTPUT", input);
	on_filter_change(filter, 0);
}

static void on_width_change(int, void*) {
	if(width % 2 == 0)
		width++;
	on_filter_change(filter, 0);
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