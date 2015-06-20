#include <cv.h>
#include <highgui.h>
#include <iostream>

/*
 * Displays the images in this orientation
 * __________________
 * |       ||       |
 * | input || thres |
 * |_______||_______|
 * |       ||       |
 * | trans ||  out  |
 * |_______||_______|
 */
void showImages (cv::Mat input, cv::Mat trans, cv::Mat thres, cv::Mat out)
{
	cv::Mat output(input.rows*2, input.cols*2, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* dPtr = output.ptr();
	unsigned char* trPtr = trans.ptr();
	unsigned char* thPtr = thres.ptr();
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = out.ptr();
	for (int r = 0; r < input.rows; r++)
	{
		for (int c = 0; c < input.cols; c++)
		{

			dPtr[3 * (r * 2 * input.cols + c)] = inPtr[3 * (r * input.cols + c)];
			dPtr[3 * (r * 2 * input.cols + c) + 1] = inPtr[3 * (r * input.cols + c) + 1];
			dPtr[3 * (r * 2 * input.cols + c) + 2] = inPtr[3 * (r * input.cols + c) + 2];

			dPtr[3 * ((r + input.rows) * 2 * input.cols + c)] = trPtr[3 * (r * input.cols + c)];
			dPtr[3 * ((r + input.rows) * 2 * input.cols + c) + 1] = trPtr[3 * (r * input.cols + c) + 1];
			dPtr[3 * ((r + input.rows) * 2 * input.cols + c) + 2] = trPtr[3 * (r * input.cols + c) + 2];

			dPtr[3 * (r * 2 * input.cols + (c + input.cols))] = thPtr[3 * (r * input.cols + c)];
			dPtr[3 * (r * 2 * input.cols + (c + input.cols)) + 1] = thPtr[3 * (r * input.cols + c) + 1];
			dPtr[3 * (r * 2 * input.cols + (c + input.cols)) + 2] = thPtr[3 * (r * input.cols + c) + 2];

			dPtr[3 * ((r + input.rows) * 2 * input.cols + (c + input.cols))] = outPtr[3 * (r * input.cols + c)];
			dPtr[3 * ((r + input.rows) * 2 * input.cols + (c + input.cols)) + 1] = outPtr[3 * (r * input.cols + c) + 1];
			dPtr[3 * ((r + input.rows) * 2 * input.cols + (c + input.cols)) + 2] = outPtr[3 * (r * input.cols + c) + 2];
		}
	}
	cv::imshow("Thresholding", output);
	cv::waitKey(0);
}

/*
 * First step of processing
 * Do any preliminary processing
 * Get information about the whole image (eg average values of colors)
 *
 * This function calculates new rgb values
 */
cv::Mat transformImage(cv::Mat input)
{

	double minRed = 100000;
	double maxRed = -100000;

	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();
	double midPtr[input.rows*input.cols];
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		int r = inPtr[3*i+2];
		int g = inPtr[3*i+1];
		int b = inPtr[3*i+0];
		double val = 100.0*(g-0.4*r)/(b+0.01);

		midPtr[i] = val;
		if (midPtr[i] > maxRed)
		{
			maxRed = midPtr[i];
		}
		if (midPtr[i] < minRed)
		{
			minRed = midPtr[i];
		}
	}
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		outPtr[3*i+1] = (int)(255*(midPtr[i]-minRed)/(maxRed-minRed));
	}
	return output;
}

/*
 * Second step of processing
 * Threshold colors and transform image
 *
 * This function scales the values
 */
cv::Mat thresholdImage(cv::Mat input)
{

	double minRed = 100000;
	double maxRed = -100000;

	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();
	double midPtr[input.rows*input.cols]={0};
	double mPtr[input.rows*input.cols]={0};
	for (int i = 0; i < input.rows * input.cols; i++)
	{
		int r = inPtr[3*i+2];
		int g = inPtr[3*i+1];
		int b = inPtr[3*i+0];
		double val = 100.0*(g-0.4*r)/(b+0.01);

		mPtr[i] = val;
	}
	int diff = 4;
	for (int c = diff; c < input.cols-diff; c++)
	{
		for (int r = diff; r < input.rows-diff; r++)
		{
			double val = ((2*mPtr[(r*input.cols+c)]-mPtr[((r-diff)*input.cols+c)]-mPtr[((r+diff)*input.cols+c)])+
				(2*mPtr[(r*input.cols+c)]-mPtr[(r*input.cols+c+diff)]-mPtr[(r*input.cols+c-diff)]))/100000+128;
			midPtr[r*input.cols+c] = val;
			if (val < minRed)
			{
				minRed = val;
			}
			if (val > maxRed)
			{
				maxRed = val;
			}
		}
	}
	std::cout<<(int)minRed<<" "<<(int)maxRed<<"\n";
			
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		outPtr[3*i+1] = (int)(255*(midPtr[i]-minRed)/(maxRed-minRed));
	}
	return output;
}

/*
 * Last step of processing
 * Do any other processing (eg finding blobs, getting centroids, etc)
 *
 * This function does k means clustering, then finds the cluster with the highest density
 * it marks that one green, and the others blue
 */
cv::Mat lastStep(cv::Mat input)
{
	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();

	std::vector<int> maxPos;
	int maxVal = 0;
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		outPtr[3*i+1] = inPtr[3*i+1];
		if (inPtr[3*i+1] > maxVal)
		{
			maxPos.clear();
			maxPos.push_back(i);
			maxVal = inPtr[3*i+1];
		}
		if (inPtr[3*i+1] == maxVal)
		{
			maxPos.push_back(i);
		}
	}
	for (int i = 0; i < maxPos.size(); i++)
	{
		outPtr[3*maxPos[i]] = 255;
	}
	
	return output;
}

int main (int argc, char* argv[])
{
	cv::Mat inImg[argc - 1];
	cv::Mat transImg[argc - 1];
	cv::Mat thresImg[argc - 1];
	cv::Mat outImg[argc - 1];

	cv::Mat dispIn[argc - 1];
	cv::Mat dispTrans[argc - 1];
	cv::Mat dispThres[argc - 1];
	cv::Mat dispOut[argc - 1];
	for (int i = 0; i < argc - 1; i++)
	{
		// get the image, run and display the result of all 3 steps of processing on the image
		dispIn[i] = cv::imread(argv[i + 1], CV_LOAD_IMAGE_COLOR);
		cv::resize(dispIn[i], inImg[i], cv::Size(64, 36));
		transImg[i] = transformImage(inImg[i]);
		thresImg[i] = thresholdImage(transImg[i]);
	      	outImg[i] = lastStep(thresImg[i]);
		cv::resize(transImg[i], dispTrans[i], cv::Size(640, 360), 0, 0, cv::INTER_NEAREST);
		cv::resize(thresImg[i], dispThres[i], cv::Size(640, 360), 0, 0, cv::INTER_NEAREST);
		cv::resize(outImg[i], dispOut[i], cv::Size(640, 360), 0, 0, cv::INTER_NEAREST);
		//std::cout<<total[2]<<" "<<total[1]<<"\n";
		showImages(dispIn[i], dispTrans[i], dispThres[i], dispOut[i]);
	}
	
}




