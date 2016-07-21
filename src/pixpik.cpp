#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<unordered_set>
 
cv::Mat src;
 
enum Thresh_Values{ MIN_B, MAX_B, MIN_G, MAX_G, MIN_R, MAX_R };
//TODO: These should go in a config file accessible to all programs involved
int tolerance = 20;
double thresh_values[6];
int min_thresh, max_thresh;
 
void resize_sane(cv::Mat& src, cv::Mat& dst, int max_size){
 
	//divide by larger and larger ints until both are under max_size
	int scale = 1;
	cv::Size dst_size;
 
	do {
		//we want to try and preserve aspect ratio, but truncating a float to an int shouldn't really matter
		dst_size.width = (int)(src.cols / scale);
		dst_size.height = (int)(src.rows / scale);
		
		scale++;
	} while((dst_size.width > max_size) || (dst_size.height > max_size));

	cv::resize(src, dst, dst_size);
 
}
 
//two points to draw the rectangle
cv::Point tl = cv::Point(0,0);
cv::Point br = cv::Point(0,0);
bool start_draw = false;

//function to be called on mouse event to draw rectangles
void draw_rect(int event, int x, int y, int flags, void* param){

	if(event == CV_EVENT_LBUTTONDOWN) {
		if(!start_draw) {
			tl = cv::Point(x, y);
			start_draw = true;
		}
 
		else { 
			br = cv::Point(x, y);
			start_draw = false;
 
			//stuff to find ROI and threshold values
			cv::Rect roi;
			cv::Mat thresh_roi;
 
			//separate the area of the image into a mat so we can find good thresholding values
			roi = cv::Rect (tl.x, tl.y, x - tl.x , y - tl.y); 
			thresh_roi = src(roi).clone();

			//split into channels to find the min anx max of each channel
			std::vector<cv::Mat> thresh_channels;
			split(thresh_roi, thresh_channels);

			//cycle through each channel and find the min and max
			std::unordered_set<std::string> values;
			for(int x = 0; x < thresh_roi.cols; x++){
				for(int y = 0; y < thresh_roi.rows; y++){
					std::stringstream ss;
					ss << "[" << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[0] << "," << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[1] <<"," << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[2] << "],";
				values.insert(ss.str());
					
				}
			}
			for(auto i = values.begin(); i != values.end(); i++){
				std::cout << *i << std::endl;
			}
		}
	}

	if( (event == CV_EVENT_MOUSEMOVE) && start_draw ){
 
		cv::Mat draw = src.clone();
 
		//draw the rectangle 
		rectangle(draw, tl, cv::Point(x, y), cv::Scalar(255, 255, 0), 2, 8);

		cv::imshow("draw", draw);
	}

	if(event == CV_EVENT_LBUTTONDBLCLK){
		cv::Mat draw;
		draw = src.clone();
		rectangle(draw, tl, cv::Point(x, y), cv::Scalar(255,255, 0), 3, 8);
		cv::imshow("draw", draw);
	}
 
 
}
		
int main(int argc, char **argv){

	src = cv::imread(argv[1], cv::IMREAD_COLOR); 
	
	resize_sane(src, src, 800);

	cv::namedWindow("draw", 1);
 
	cv::setMouseCallback("draw", draw_rect, NULL);
	
	cv::imshow("draw", src);
	cv::waitKey(0);
}
