#include<cv.h>
#include<highgui.h>
#include<cstdio>
#include"opencv2/core/mat.hpp"
#include<cmath>
#include "opencv2/features2d/features2d.hpp"
using namespace std;
using namespace cv;
//suffix CB means an image with this applied to it
void SimplestCB(Mat& in, Mat& out, float percent)
	 {
	assert(in.channels() == 3);
	assert(percent > 0 && percent < 100);
	float half_percent = percent / 200.0f;
	vector<Mat> tmpsplit; split(in,tmpsplit);
	for(int i=0;i<3;i++) {
		//find the low and high precentile values (based on the input percentile)
		Mat flat; tmpsplit[i].reshape(1,1).copyTo(flat);
		cv::sort(flat,flat,CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
		int lowval = flat.at<uchar>(cvFloor(((float)flat.cols) * half_percent));
		int highval = flat.at<uchar>(cvCeil(((float)flat.cols) * (1.0 - half_percent)));
		std::cout << lowval << " " << highval << std::endl;
		//saturate below the low percentile and above the high percentile
		tmpsplit[i].setTo(lowval,tmpsplit[i] < lowval);
		tmpsplit[i].setTo(highval,tmpsplit[i] > highval);
		//scale the channel
		normalize(tmpsplit[i],tmpsplit[i],0,255,NORM_MINMAX);
		}
	merge(tmpsplit,out);
	} 
//suffix PSZ means an image with this applied to it
void posterize(Mat& inWrapper,Mat& outputPSZImg, int outputSizeX, int outputSizeY, int clusterCount )
{	//
	//Declaring all of the variables used in kMeans
	//inWrapper is the input image.
	//defines a new size to resize to
	Size outputSize(outputSizeX, outputSizeY);
	//simple resize
	resize(inWrapper, inWrapper, outputSize, 0, 0, INTER_NEAREST);
	//This creates a new empty matrix of the proper size
	Mat samples(inWrapper.rows*inWrapper.cols, 3, CV_32F);
	//nested for populates the matrix in the format openCV wants
	//most of the rest of the code i jus copied from stack overflow. I have a sort of vague idea of what it does. If you really want to find out and document it, you're welcome to do so yourself you lazy bum.
	for(int y = 0; y < inWrapper.rows; y++)
		for(int x = 0; x < inWrapper.cols; x++)
			for ( int z = 0; z< 3; z++)
				samples.at<float>(y+x*inWrapper.rows, z) = inWrapper.at<Vec3b>(y,x)[z];
	Mat labels;
	int attempts = 3;
	Mat centers;
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
	Mat kmeanOut(inWrapper.size() , inWrapper.type() );
	for(int y = 0; y  < inWrapper.rows; y++)
		for(int x = 0; x < inWrapper.cols; x++)
		{
			int cluster_idx = labels.at<int>(y+x*inWrapper.rows, 0);
			kmeanOut.at<Vec3b>(y,x)[0]= centers.at<float>(cluster_idx, 0);	

			kmeanOut.at<Vec3b>(y,x)[1]= centers.at<float>(cluster_idx, 1);	

			kmeanOut.at<Vec3b>(y,x)[2]= centers.at<float>(cluster_idx, 2);	
		}
	resize(kmeanOut, outputPSZImg, outputSize, 0, 0, INTER_NEAREST); 
}
void redChannelChange(Mat& inImg, Mat& outImg)
{
	for(int y = 0; y < inImg.cols; y++)
	{
		for(int x = 0; x < inImg.rows; x++)
		{
			Vec3b redness = inImg.at<Vec3b>(x ,y);
			outImg.at<uchar>(x,y) = redness.val[2];
		}
	}
}

int main(int argc, char** argv)
{
	
	for (int i = 0; i < argc - 1; i++)
	{
		//initialize images
		//original image
		Mat image;
		Mat image_processed;
		Mat image_posterized;
		Mat image_redChannelChange;
		Mat image_threshold;
		Mat image_blobs;
		//final image
		image = imread(argv[i+1]);
		//applying cb function to image
		posterize(image, image_posterized, 64, 36, 5);
		cvtColor(image_posterized, image_redChannelChange, CV_RGB2GRAY);
		redChannelChange(image_posterized, image_redChannelChange);
		double maxPixVal;
		minMaxLoc(image_redChannelChange, nullptr, &maxPixVal);
		threshold(image_redChannelChange, image_threshold, maxPixVal - .0001  , 255, 1);
		Size final_Size(640, 360);
		resize(image_threshold, image_threshold, final_Size, 0,0, INTER_NEAREST);
		
		SimpleBlobDetector::Params params;
		params.filterByArea = false;
		SimpleBlobDetector detector(params);
	  	//Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);	
		vector<KeyPoint> keypoints;
		detector.detect(image_threshold, keypoints);
		drawKeypoints( image_threshold, keypoints, image_blobs, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		resize(image_blobs, image_processed, final_Size, 0, 0, INTER_NEAREST);
		resize(image, image, final_Size);
		//displaying images
		namedWindow("Original Image", WINDOW_AUTOSIZE);
		namedWindow("Processed Image", WINDOW_AUTOSIZE);
		imshow("Processed Image", image_processed);
		imshow("Original Image", image);
		waitKey(0);
	}
	return (0);
}

