#include <cv.h>
#include <algorithm>
#include <highgui.h>
#include <iostream>

using namespace cv;

cv::Mat kmeansImgWrapper(Mat inWrapper,int outputTempSizeX, int outputTempSizeY, int clusterTempCount )
{	
	//Declaring all of the variables used in kMeans
	//inWrapper is the input image.
	int outputSizeX =  outputTempSizeX;
	int outputSizeY = outputTempSizeY;
	int clusterCount = clusterTempCount;
	//defines a new size to resize to
	Size outputSize(outputSizeX, outputSizeY);
	//simple resize
	resize(inWrapper, inWrapper, outputSize, 0, 0, INTER_NEAREST);
	//This creates a new empty matrix of the proper size
	Mat samples(inWrapper.rows*inWrapper.cols, 3, CV_32F);
	//nested for populates the matrix in the format openCV wants
	//most of the rest of the code i just copied from stack overflow. I have a sort of vague idea of what it does. If you really want to find out and document it, you're welcome to do so yourself you lazy bum.
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

	return kmeanOut;
}

cv::Mat equalColorHist(cv::Mat& img, bool red, bool green, bool blue)
{
	std::vector<cv::Mat> channels;
	cv::split(img, channels);

	if (blue)
	{
		cv::equalizeHist(channels[0], channels[0]);
	}
	if (green)
	{
		cv::equalizeHist(channels[1], channels[1]);
	}
	if (red)
	{
		cv::equalizeHist(channels[2], channels[2]);
	}

	cv::Mat result;
	cv::merge(channels, result);
	return result;
}
struct Bin
{
	int x;
	int y;
	int weight;

	Bin(int x, int y, int weight) :
		x(x),
		y(y),
		weight(weight)
	{
	}
};

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
		double val = 4*g-2*r-b;
		//double val = std::max(0, std::min(255, 128+40*r/(g+1) - 120*g/(b+1)));

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
	std::cout<<maxRed<<" "<<minRed<<" ayylmao\n";
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		outPtr[3*i+1] = (int)(255*(midPtr[i]-minRed)/(maxRed-minRed));
	}
	output = equalColorHist(input,false,true,true);
	return output;
}

int maxR;
int rx;
int ry;
int yx;
int yy;

/*
 * Second step of processing
 * Threshold colors and transform image
 *
 * This function scales the values
 */
cv::Mat thresholdImage(cv::Mat input)
{

	double minRed = 10000;
	double maxRed = -10000;
	maxR = -1;
	rx = 0;
	ry = 0;
	yx = 0;
	yy = 0;
	int tempR;

	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();
	double midPtr[input.rows*input.cols]={0};
	double mPtr[input.rows*input.cols]={0};
	for (int i = 0; i < input.cols*input.rows; i++)
	{
		int b = inPtr[3*i], g = inPtr[3*i+1], r = (int)inPtr[3*i+2];
		
		tempR = 128+40*r/(g+1) - 120*g/(b+1);

		// find red buoy
		if (tempR > maxR)
		{
			maxR = tempR;
			rx = i % input.cols;
			ry = i / input.cols;
		}
	}
	int maxY = -1;
	for (int i = std::max(0,(int)(input.cols*(ry-0.2*input.rows))); i < std::min(input.rows*input.cols,(int)(input.cols*(ry+0.2*input.rows))); i++)
	{
		
		int b = inPtr[3*i], g = inPtr[3*i+1], r = (int)inPtr[3*i+2];

		int tempY = std::max(0, std::min(255, 128+(r+g-b)/4));

		// find red buoy
		if (tempY > maxY)
		{
			maxY = tempY;
			yx = i % input.cols;
			yy = i / input.cols;
		}
	}
	float maxG = -1000;
	float minG = 1000;
	float tPtr[input.rows*input.cols];
	float qPtr[input.rows*input.cols];
	int diffDist = 8;
	int gx = 0;
	int gy = 0;
	bool adjust = true;
	float gDistX = 0.05;
	float gDistY = 0.3;
	std::vector<int> values;
	int totG = 0;
	float tempM = -100;
	input = equalColorHist(input,false,false,false);

	inPtr = input.ptr();
	for (int i = 0; i < input.rows*input.cols; i++)
	{
		int b = inPtr[3*i], g = inPtr[3*i+1], r = (int)inPtr[3*i+2];
		tPtr[i] = (g-0.4*r)/(b+3.01); 
		//tPtr[i] = r;
		//tPtr[i] = (g+0.4*r)/(b+3.0*r+3.01)-0.004*r;
		//tPtr[i] = 40.0*r/(g+1.0) - 120.0*g/(b+1.0);
		if (tPtr[i] > tempM)
		{
			tempM = tPtr[i];
		}
		totG += tPtr[i];
		values.push_back(tPtr[i]);
	}
//	cv::Mat histothing;
//	cv::cvtColor(input, histothing, cv::COLOR_BGR2GRAY);
//	for (int i = 0; i < input.rows*input.cols; i++)
//	{
//		histothing.ptr()[i] = tPtr[i];
//	}
//	cv::equalizeHist(histothing, histothing);
//	for (int i = 0; i < input.rows*input.cols; i++)
//	{
//		tPtr[i] = histothing.ptr()[i];
//	}
	// find difference between pixel and those around it (buoy should stand out from water a lot)
	for (int c = diffDist; c < input.cols-diffDist; c++)
	{
		for (int r = diffDist; r < input.rows-diffDist; r++)
		{
			// make the value equal to how much it stands out from its horizontal or vertical neighbors, whichever is less
			float vert = (2*tPtr[(r*input.cols+c)]-tPtr[((r-diffDist)*input.cols+c)]-tPtr[((r+diffDist)*input.cols+c)]);
			float hori = (2*tPtr[(r*input.cols+c)]-tPtr[(r*input.cols+c+diffDist)]-tPtr[(r*input.cols+c-diffDist)]);
			float weak = (std::abs(vert) < std::abs(hori)) ? vert+128 : hori+128;
			qPtr[(r*input.cols+c)] = weak;
			// find green buoy and make sure it is near the same height as red, but not too close to red or yellow
			if ((adjust) || ((std::abs(c-rx) > input.cols*gDistX) && (std::abs(c-yx) > input.cols*gDistX)))
			{
				if (weak < minG)
				{
					minG = weak;
				}
				if (weak > maxG)
				{
					gx = c;
					gy = r;
					maxG = weak;
				}
			}
			else
			{
				qPtr[(r*input.cols+c)] /= 3;
			}
		}
	}
	std::sort(values.begin(), values.end());
	std::cout<<"avg: "<<(1.0*totG/(values.size()))<<"  med: "<<values[values.size()/2]<<"  top: "<<tempM<<"\n";
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		outPtr[3*i+1] = (int)(255*(qPtr[i]-minG)/(maxG-minG));
	}
	output.ptr()[3*(gy*input.cols+gx)+2] = 255;
	//output = kmeansImgWrapper(input, 128,72,10);
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
/*	for (int i = 0; i< input.rows * input.cols; i++)
	{
		outPtr[3*i+1] = inPtr[3*i+1];
		if ((std::abs(i%input.cols -rx) > input.cols*0.2) && (std::abs(i/input.cols -ry) < input.rows*0.4))
		{
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
	}
	*/
	std::vector<Bin> yellow;
	for (int i = 0; i < input.rows*input.cols; i++)
	{
		int b = inPtr[3*i], g = inPtr[3*i+1], r = (int)inPtr[3*i+2];
	//	if ((std::abs(i%input.cols -yx) > input.cols*0.1) && (std::abs(i%input.cols -rx) > input.cols*0.1) && (std::abs(i/input.cols -ry) < input.rows*0.2))
		{
			yellow.push_back(Bin(i%input.cols, i/input.cols, g));
			outPtr[3*i+1] = g;
		}
	/*	else
		{
			outPtr[3*i] = g;
		}
	*/}

	// sort by weight
	std::sort(yellow.begin(), yellow.end(), [](Bin a, Bin b){return a.weight > b.weight;});

	int numBins = 0;
	float bins[3][2] = {{0}};
	int binP[3][3] = {{0}};
	// find best 3 not close to eachother
	for (Bin a : yellow)
	{
		if (numBins == 2) break;
		float dx = ((float)a.x - input.cols/2) / input.cols;
		float dy = ((float)input.rows/2 - a.y) / input.rows;
		bool tooClose = false;
		for (int i = 0; i < numBins; i++)
			if (sqrt(pow(dx - bins[i][0], 2) + pow(dy - bins[i][1], 2)) < .1)
				tooClose = true;
		if (!tooClose)
		{
			bins[numBins][0] = dx;
			bins[numBins][1] = dy;
			binP[numBins][0] = a.x;
			binP[numBins][1] = a.y;
			binP[numBins][2] = a.weight;
			numBins++;
		}
	}
	for (int i = 0; i < numBins; i++)
	{
		std::cout<<"MAX  "<<binP[i][0]<<" "<<binP[i][1]<<" "<<(binP[i][1]*input.cols+binP[i][0])<<"\n";
		outPtr[3*(binP[i][1]*input.cols+binP[i][0])] = 255/(i+1);
		std::cout<<binP[i][2]<<"\n\n";
		outPtr[3*(binP[i][1]*input.cols+binP[i][0])+1] = 0;
	}
	outPtr[3*(input.cols*ry+rx)+2] = 255;
	outPtr[3*(input.cols*ry+rx)+1] = 0;
	outPtr[3*(input.cols*ry+rx)+0] = 0;
	outPtr[3*(input.cols*yy+yx)+2] = 255;
	outPtr[3*(input.cols*yy+yx)+1] = 255;
	outPtr[3*(input.cols*yy+yx)+0] = 0;
	
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
		cv::resize(dispIn[i], inImg[i], cv::Size(640, 360));
		transImg[i] = transformImage(inImg[i]);
		cv::resize(dispIn[i], inImg[i], cv::Size(128, 72));
		thresImg[i] = thresholdImage(inImg[i]);
	      	outImg[i] = lastStep(thresImg[i]);
		cv::resize(transImg[i], dispTrans[i], cv::Size(640, 360), 0, 0, cv::INTER_NEAREST);
		cv::resize(thresImg[i], dispThres[i], cv::Size(640, 360), 0, 0, cv::INTER_NEAREST);
		cv::resize(outImg[i], dispOut[i], cv::Size(640, 360), 0, 0, cv::INTER_NEAREST);
		//std::cout<<total[2]<<" "<<total[1]<<"\n";
		showImages(dispIn[i], dispTrans[i], dispThres[i], dispOut[i]);
	}
	
}




