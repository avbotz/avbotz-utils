#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unordered_set>
#include <string>
 
cv::Mat src;
 
enum Thresh_Values{ MIN_B, MAX_B, MIN_G, MAX_G, MIN_R, MAX_R };
//TODO: These should go in a config file accessible to all programs involved
int tolerance = 20;
double thresh_values[6];
int min_thresh, max_thresh;
//std::iterator<string> id;

cv::Mat zoom;
//std::unordered_set<std::string> values;
 
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
void increase_sane(cv::Mat& src, cv::Mat& dst, int min_size){
 
	//divide by larger and larger ints until both are under min_size
	int scale = 1;
	cv::Size dst_size;
 
	do {
		//we want to try and preserve aspect ratio, but truncating a float to an int shouldn't really matter
		dst_size.width = (int)(src.cols * scale);
		dst_size.height = (int)(src.rows * scale);
		
		scale++;
	} while((dst_size.width < min_size) || (dst_size.height < min_size));

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
 
			roi = cv::Rect (std::min(tl.x, x), std::min(tl.y, y), std::max(x - tl.x, tl.x - x) , std::max(y - tl.y, tl.y - y));
			thresh_roi = zoom(roi).clone();

			std::vector<cv::Mat> thresh_channels;
			split(thresh_roi, thresh_channels);

			for(int x = 0; x < thresh_roi.cols; x++){
				for(int y = 0; y < thresh_roi.rows; y++){
					std::stringstream ss;
                             ss << "[" << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[2] << "," << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[1] <<"," << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[0] << "],";
					std::cout << ss.str() << std::endl;
                               //values.insert(ss.str());
				}
			}
		}
	}

	if( (event == CV_EVENT_MOUSEMOVE) && start_draw ){
 
		cv::Mat draw = zoom.clone();
 
		//draw the rectangle 
		rectangle(draw, tl, cv::Point(x, y), cv::Scalar(255, 255, 0), 2, 8);

		cv::imshow("zoom", draw);
	}

	if(event == CV_EVENT_LBUTTONDBLCLK){
		cv::Mat draw;
		draw = zoom.clone();
		rectangle(draw, tl, cv::Point(x, y), cv::Scalar(255,255, 0), 3, 8);
		cv::imshow("zoom", draw);
	}
 
 
}
		
void zoom_rect(int event, int x, int y, int flags, void* param){

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
 
			roi = cv::Rect (std::min(tl.x, x), std::min(tl.y, y), std::max(x - tl.x, tl.x - x) , std::max(y - tl.y, tl.y - y));
			zoom = src(roi).clone();
			
			increase_sane(zoom, zoom, 300);	
			cv::imshow("zoom", zoom);	

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

void select_rect(int event, int x, int y, int flags, void* param){

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
 
			roi = cv::Rect (std::min(tl.x, x), std::min(tl.y, y), std::max(x - tl.x, tl.x - x) , std::max(y - tl.y, tl.y - y));
			zoom = src(roi).clone();
			thresh_roi = src(roi).clone();
			
			std::vector<cv::Mat> thresh_channels;
			split(thresh_roi, thresh_channels);

			for(int x = 0; x < thresh_roi.cols; x++){
				for(int y = 0; y < thresh_roi.rows; y++){
					std::stringstream ss;
                             ss << "[" << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[2] << "," << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[1] <<"," << (int)thresh_roi.at<cv::Vec3b>(cv::Point(x, y))[0] << "],";
					std::cout << ss.str() << std::endl;
                               //values.insert(ss.str());
				}
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

int main(int argc, char **argv) {
	int counter = 0;
	std::string z = "-z";
	for(int n = 1; n < argc; n++) {
		if(argv[n] == z) {
			counter++;
		}
	}
	if(counter>0) {
		cv::namedWindow("draw", 1);
		cv::namedWindow("zoom", 1);
		cv::setMouseCallback("draw", zoom_rect, NULL);
		cv::setMouseCallback("zoom", draw_rect, NULL);
	}
	if(counter == 0) {
		cv::namedWindow("draw", 1);
                cv::setMouseCallback("draw", select_rect, NULL);
	}

	for(int i = 1; i < argc; i++) {
		src = cv::imread(argv[i], cv::IMREAD_COLOR); 

		resize_sane(src, src, 800);
	
		cv::imshow("draw", src);
		cv::waitKey(0);
	}
}
