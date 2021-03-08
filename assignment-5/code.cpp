#include <bits/stdc++.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <filesystem>
#define MAX_N 15
using namespace cv;
using namespace std;

class struct_element{
public:
	bool** kernel = nullptr;
	int h = 0, w = 0;
	int centerH = 0, centerW = 0;
	struct_element(){

	}
	struct_element(bool** kernel,int h,int w,int centerH,int centerW){
		this->h = h;
		this->w = w;
		this->centerH = centerH;
		this->centerW = centerW;
		this->kernel = new bool*[h];
		for (int i = 0; i < h; i++)
		{
			this->kernel[i] = new bool[w];
			for (int j = 0; j < w; j++)
			{
				this->kernel[i][j] = kernel[i][j];
			}
			
		}
	}
	struct_element(const struct_element &se)
	{
		h = se.h;
		w = se.w;
		centerH = se.centerH;
		centerW = se.centerW;
		kernel = new bool *[h];
		for (int i = 0; i < h; i++)
		{
			kernel[i] = new bool[w];
			for (int j = 0; j < w; j++)
			{
				kernel[i][j] = se.kernel[i][j];
			}
		}
	}
	struct_element reflection() const{
		struct_element op(*this);
		for (int i = 0; i < op.h; i++)
		{
			for (int j = 0; j < op.w; j++)
			{
				op.kernel[i][j] = this->kernel[op.h - 1 - i][op.w - 1 - j];
			}
		}
		op.centerH = op.h - 1 - this->centerH;
		op.centerW = op.w - 1 - this->centerW;
		return op;
	}
	~struct_element(){
		for (int i = 0; i < h; i++)
		{
			delete this->kernel[i];
		}
		delete this->kernel;
	}
};
namespace operations{

	bool check(int x, int y, int n, int m) {
		return x>=0 and y>=0 and x<n and y<m;
	}

	Mat dilate(const Mat& input,const struct_element& se) {
		Mat output = input.clone();
		struct_element se_hat(se.reflection());

		for(int x=0; x<input.rows; x++) {
			for(int y=0; y<input.cols; y++) {
				bool val = 0, curr;
				for(int i=0; i<se_hat.h; i++) {
					for(int j=0; j<se_hat.w; j++) {
						int xx = x + (i - se_hat.centerH);
						int yy = y + (j - se_hat.centerW);
						if( !check(xx, yy, input.rows, input.cols) || se.kernel[i][j] == 0)
							continue;
						curr = (input.at<uchar>(xx, yy) > 0) and se.kernel[i][j];
						val |= curr;
					}
				}
				output.at<uchar>(x, y) = 255 * (int)val;
			}
		}

		return output;
	}

	Mat erode(const Mat& input, const struct_element& se) {
		Mat output = input.clone();
		
		for(int x=0; x<input.rows; x++) {
			for(int y=0; y<input.cols; y++) {
				bool val = 1, curr;
				for(int i=0; i<se.h; i++) {
					for(int j=0; j<se.w; j++) {
						int xx = x + (i - se.centerH);
						int yy = y + (j - se.centerW);
						if(!check(xx, yy, input.rows, input.cols)){
							val = 0;
							break;
						}
						if(se.kernel[i][j] == 0){
							continue;
						}
						curr = (input.at<uchar>(xx,yy)>0) and se.kernel[i][j];
						val &= curr;
					}
				}
				output.at<uchar>(x,y) = 255 * (int)val;
			}
		}


		return output;
	}

	Mat openclose(const Mat& input, const struct_element& se)
	{

		return erode(dilate(input, se),se);	
	}

	Mat closeopen(const Mat& input, const struct_element& se) {

		return dilate(erode(input,se),se);
	}
}
namespace change{
	cv::Mat input;
	const int id_max = 4, type_max = 4;
	vector<struct_element> elements;
	int id, type;
	void init(string input_image_path){
		change::input = cv::imread(input_image_path, 0);
		bool **kernel = new bool *[MAX_N];
		for (size_t i = 0; i < MAX_N; i++)
		{
			kernel[i] = new bool[MAX_N];
			for (size_t j = 0; j < MAX_N; j++)
			{
				kernel[i][j] = 1;
			}
		}
		change::elements.push_back(struct_element(kernel, 1, 2, 0, 0));
		change::elements.push_back(struct_element(kernel, 3, 3, 1, 1));
		kernel[0][0] = kernel[0][2] = kernel[2][0] = kernel[2][2] = 0;
		change::elements.push_back(struct_element(kernel, 3, 3, 1, 1));
		kernel[0][0] = kernel[0][2] = kernel[2][0] = kernel[2][2] = 1;
		change::elements.push_back(struct_element(kernel, 9, 9, 4, 4));
		change::elements.push_back(struct_element(kernel, 15, 15, 7, 7));
	}
	void on_change(int, void*) {
		if(id>=elements.size()){
			throw runtime_error("given structuring element is not present");
		}

		Mat output;
		switch(type) {
			case 1:
				output = operations::dilate(input, elements.at(id));
				break;
			case 2:
				output = operations::erode(input, elements.at(id));
				break;
			case 3:
				output = operations::openclose(input, elements.at(id));
				break;
			case 4:
				output = operations::closeopen(input, elements.at(id));
				break;
			default:
				return;
		}
		cv::imshow("output",output);
	}
};

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		cout << "usage ./code <path to input image>\n";
		return -1;
	}
	change::init(argv[1]);
	namedWindow("Structure Tracker", WINDOW_AUTOSIZE);
	createTrackbar("Structure", "Structure Tracker", &change::id, change::id_max, change::on_change);
    createTrackbar("Transformation", "Structure Tracker", &change::type, change::type_max, change::on_change);
    imshow("Structure Tracker", change::input);
    waitKey(0);
    return 0;
}