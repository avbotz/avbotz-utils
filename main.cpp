#include <cv.h>
#include <highgui.h>

int total[3] = {0, 0, 0};
int avg[3];
int minRed = 255;
int maxRed = 0;

int minBlue = 255;
int maxBlue = 0;

int minGreen = 255;
int maxGreen = 0;

int maxMove = 10;
int minSep = 10;

int avgRX;
int avgRY;
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
	total[0] = 1;
	total[1] = 1;
	total[2] = 1;

	minRed = 255;
	maxRed = 0;
	minBlue = 255;
	maxBlue = 0;
	minGreen = 255;
	maxGreen = 0;

	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		int inBlue = inPtr[3*i];
		int inGreen = inPtr[3*i + 1];
		int inRed = inPtr[3*i + 2];

		int r = inRed, g = inGreen, b = inBlue;
		// calculate a new red value based on the rgb value of the pixel
		int outRed = (float) 1 * r / (g+1) * std::max((2*r - 1*g - 0*b) / 2 + 30, 0);
		outRed = (outRed > 0) ? outRed : 0;
		outRed = (outRed < 255) ? outRed : 255;
		// get the max and min values of the image for scaling in the next step
		if (outRed > maxRed)
		{
			maxRed = outRed;
		}
		if (outRed < minRed)
		{
			minRed = outRed;
		}

		// I just wrote random stuff for this one
		// it doesn't work
	//	int outGreen = ((int)inGreen - ((int)inRed + 4*(int)inBlue)/4);
	//	outGreen = (outGreen > 0) ? outGreen : 0;
	//	outGreen = (outGreen < 255) ? outGreen : 255;
	//	if (outGreen > maxGreen)
	//	{
	//		maxGreen = outGreen;
	//	}
	//	if (outGreen < minGreen)
	//	{
	//		minGreen = outGreen;
	//	}

		// who knows what this does
		// probably yellow buoys eventually
		int outBlue = 0;

		int outGreen = 0;

		// get totals of each color in order to get average
		total[2] += outRed;
		total[1] += outGreen;
		total[0] += outBlue;

		// set values of output image
		outPtr[3*i + 2] = outRed;
		outPtr[3*i + 1] = outGreen;
		outPtr[3*i + 0] = outBlue;
	}
	avg[2] = total[2] / (output.rows * output.cols);
	avg[1] = total[1] / (output.rows * output.cols);
	avg[0] = total[0] / (output.rows * output.cols);
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
	total[0] = 1;
	total[1] = 1;
	total[2] = 1;
	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();

	int totPos[] = {0, 0};
	int totRed = 0;

	for (int i = 0; i< input.rows * input.cols; i++)
	{
		int inBlue = inPtr[3*i];
		int inGreen = inPtr[3*i + 1];
		int inRed = inPtr[3*i + 2];

		// sets all pixels with lower than average values to 0
		int outBlue = (inBlue > avg[0]) ? inBlue : 0;
		int outGreen = (inGreen > avg[1]) ? inGreen : 0;
		int outRed = (inRed > avg[2]) ? inRed : 0;

		// scale values so that the lowest value in the image is 0, the highest is 255
		outBlue = (int)((255.0 * outBlue) / (1 + maxBlue - minBlue));
		outGreen = (int)((255.0 * outGreen) / (1 + maxGreen - minGreen));
		outRed = (int)((255.0 * outRed) / (1 + maxRed - minRed));

		outBlue = (long)pow(outBlue, 4) / (long)pow(255, 3);
		outGreen = (long)pow(outGreen, 4) / (long)pow(255, 3);
		outRed = (long)pow(outRed, 4) / (long)pow(255, 3);
		// get the new total red, green, and blue values to find a weighed average in the next step
		total[0] += outBlue;
		total[1] += outGreen;
		total[2] += outRed;

		if (outRed > 0)
		{
			totPos[0] += outRed * (i % input.cols);
			totPos[1] += outRed * (i / input.cols);
			totRed += outRed;
	//		totRed++;
	//		totPos[0] += i % input.cols;
	//		totPos[1] += i / input.cols;
		}

		outPtr[3*i] = outBlue;
		outPtr[3*i + 1] = outGreen;
		outPtr[3*i + 2] = outRed;
	}
	avgRX = totPos[0] / totRed;
	avgRY = totPos[1] / totRed;
	outPtr[3 * (avgRY * input.cols + avgRX) + 1] = 255;
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

	long totalRed[2] = {0, 0};
	long totalGreen[2] = {0, 0};
	long totalBlue[2] = {0, 0};
	int centroids[6][3];
	int totalCentroids = 0;
	for (int n = 1; n <= 6; n++)
	{
		int centroid[n][2];
		
		// I should figure out a better way to do this
		switch (n)
		{
			case 1:
				centroid[0][0] = avgRX;
				centroid[0][1] = avgRY;
				break;
			case 2:
				centroid[0][0] = avgRX;
				centroid[0][1] = avgRY - 5;
				centroid[1][0] = avgRX;
				centroid[1][1] = avgRY + 5;
				break;
			case 3:
				centroid[0][0] = avgRX;
				centroid[0][1] = avgRY + 5;
				centroid[1][0] = avgRX - 5;
				centroid[1][1] = avgRY - 5;
				centroid[2][0] = avgRX + 5;
				centroid[2][1] = avgRY - 5;
				break;
			case 4:
				centroid[0][0] = avgRX + 5;
				centroid[0][1] = avgRY + 5;
				centroid[1][0] = avgRX + 5;
				centroid[1][1] = avgRY - 5;
				centroid[2][0] = avgRX - 5;
				centroid[2][1] = avgRY - 5;
				centroid[3][0] = avgRX - 5;
				centroid[3][1] = avgRY + 5;
				break;
			case 5:
				centroid[0][0] = avgRX - 5;
				centroid[0][1] = avgRY - 5;
				centroid[1][0] = avgRX - 5;
				centroid[1][1] = avgRY + 5;
				centroid[2][0] = avgRX + 5;
				centroid[2][1] = avgRY + 5;
				centroid[3][0] = avgRX + 5;
				centroid[3][1] = avgRY - 5;
				centroid[4][0] = avgRX + 5;
				centroid[4][1] = avgRY;
				break;
			case 6:
				centroid[0][0] = avgRX + 5;
				centroid[0][1] = avgRY + 5;
				centroid[1][0] = avgRX - 5;
				centroid[1][1] = avgRY + 5;
				centroid[2][0] = avgRX;
				centroid[2][1] = avgRY + 5;
				centroid[3][0] = avgRX - 5;
				centroid[3][1] = avgRY - 5;
				centroid[4][0] = avgRX + 5;
				centroid[4][1] = avgRY - 5;
				centroid[5][0] = avgRX;
				centroid[5][1] = avgRY - 5;
				break;
		}
		int effort = 1;
		bool useless = false;
		while (effort > 0) 
		{
			long centroidWeight[n][4];
			for (int i = 0; i < n; i++)
			{
				centroidWeight[i][0] = 0;
				centroidWeight[i][1] = 0;
				centroidWeight[i][2] = 0;
				centroidWeight[i][3] = 0;
			}
			for (int i = 0; i< input.rows * input.cols; i++)
			{
				int inBlue = inPtr[3*i];
				int inGreen = inPtr[3*i + 1];
				int inRed = inPtr[3*i + 2];
				int outBlue = inBlue;
				int outGreen = inGreen;
				int outRed = inRed;

				int minDist = 10000;
				int min = 0;
				for (int m = 0; m < n; m++)
				{
					int dists = std::abs(centroid[m][0] - (i % input.cols)) + (std::abs(centroid[m][1] - (i / input.cols)));
					if (dists < minDist)
					{
						min = m;
						minDist = dists;
					}
				}
				if (inRed > 0)
				{
					centroidWeight[min][2] += inRed;
					centroidWeight[min][0] += inRed * (i % input.cols);
					centroidWeight[min][1] += inRed * (i / input.cols);
					centroidWeight[min][3]++;
				}

				// get the weighed total of the x and y coordinates of each color
				outPtr[3*i] = outBlue;
				outPtr[3*i + 1] = outGreen;
				outPtr[3*i + 2] = outRed;
			}
			int move = 0;
			for (int m = 0; m < n; m++)
			{
				int oldX = centroid[m][0];
				int oldY = centroid[m][1];
				if (centroidWeight[m][2] != 0)
				{
					centroid[m][0] = (int)(0.5 + 1.0 * centroidWeight[m][0] / centroidWeight[m][2]);
					centroid[m][1] = (int)(0.5 + 1.0 * centroidWeight[m][1] / centroidWeight[m][2]);
					centroid[m][2] = (int)(1024.0 * centroidWeight[m][2] / centroidWeight[m][3]);
					move += std::abs(centroid[m][0] - oldX) + std::abs(centroid[m][1] - oldY);
				}
				else
				{
					useless = true;
					centroid[m][0] = 0;
					centroid[m][1] = 0;
					centroid[m][2] = 0;
					move = maxMove + 1;
				}
			}
			if ((move < maxMove) || (effort > 20))
			{
				effort = -1;
			}
			// displays the locations of the centroids at every loop
		//	std::cout<<centroidWeight[0][2]<<" "<<n<<"\n";
		//	std::cout<<centroid[0][0]<<" x"<<n<<"\n";
		//	std::cout<<centroid[0][1]<<" y"<<n<<"\n";
		//	cv::imshow("Thresholding", output);
		//	cv::waitKey(0);
			effort++;
		}
		bool crowded = false;
		for (int i = 0; i < n; i++)
		{
			for (int j = i + 1; j < n; j++)
			{
				if ((std::abs(centroid[i][0] - centroid[j][0]) + std::abs(centroid[i][1] - centroid[j][1]) < minSep) || (useless))
				{
					crowded = true;
				}
			}
		}
		if (crowded)
		{
			//std::cout<<"Crowded with "<<totalCentroids<<" centroids\n";
			break;
		}
		else
		{
			totalCentroids = n;
			for (int m = 0; m < n; m++)
			{
				centroids[m][0] = centroid[m][0];
				centroids[m][1] = centroid[m][1];
				centroids[m][2] = centroid[m][2];
			}
		}
	}

	// just use the blue channel to make a 10x10 square at the center
//	for (int m = 0; m < totalCentroids; m++)
//	{
//		for (int i = centroids[m][0] - 5; i < centroids[m][0] + 5; i++)
//		{
//			for (int j = centroids[m][1] - 5; j < centroids[m][1] + 5; j++)
//			{
//				if ((i > 0) && (i < input.cols) && (j > 0) && (j < input.rows))
//				{
//					outPtr[3*(i + j * input.cols)] = 255;
//				}
//			}
//		}
	if (totalCentroids > 0)
	{
		int maxCent = 0;
		int maxVal = centroids[0][2];
		for (int m = 0; m < totalCentroids; m++)
		{
			outPtr[3*(centroids[m][0] + centroids[m][1] * input.cols)] = 255;
			outPtr[3*(centroids[m][0] + centroids[m][1] * input.cols)+1] = 0;
			outPtr[3*(centroids[m][0] + centroids[m][1] * input.cols)+2] = 0;
			//std::cout<<centroids[m][0]<<" CENTROID TOTAL "<<centroids[m][2]<<"\n";
			if (centroids[m][2] > maxVal)
			{
				maxVal = centroids[m][2];
				maxCent = m;
			}
		}
		outPtr[3*(centroids[maxCent][0] + centroids[maxCent][1] * input.cols)] = 0;
		outPtr[3*(centroids[maxCent][0] + centroids[maxCent][1] * input.cols)+1] = 255;
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



