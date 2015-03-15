/*
hi * P.S. (pre script)
hi * For an explanation of these stupid comments,
hi * Look at the bottom
hi *
hi * P.P.S.B.P.A.S.S. ("post pre script, but pre actual script" script)
hi * The P.P.P.S. is where I tell you how to get rid of these comments
hi *
hi ****************************************************************************
hi *
hi *  Copyright whatever, I'm copying linux's comment description warranty thing
hi *
hi *  This thing is free, do whatever, "AS IS" blah blah. It might blow up your computer, it might not do anything.
hi *  Who knows. I don't care and don't sue me.
hi *
hi *  Do people include this stuff in 350 line terrible programs?
hi *  I should do something else instead of writing useless comments
hi *  I'm eating a habanero. Luke, you should try one
hi *  I have nothing to do while my mouth burns and I watch Apex Salty Suite, so I'm commenting this
hi */
#include <cv.h>
#include <highgui.h>

/*
hi * I think I'm supposed to have a description of the program up here, so here it is
hi *
hi * This basically just splits image processing into 3 steps and displays the original image, along with what the image looks like after each step.
hi * The default program does this:
hi * 	1) Find new rgb values based on the old ones
hi * 	2) Scale the values
hi * 	3) Find the centroid of each color channel
hi *
hi * Dude commenting stuff is hard
hi *
hi * How much am I supposed to comment?
hi * How stupid do I assume readers are?
hi * Should I explain how opencv Mats work?
hi * Should I say how all of the stuff in showImages works?
hi * Should I tell them that the limit of steps is 3 because my screen can only fit 4 images on my screen at once?
hi * Should I just scale the images to make them all fit?
hi * Will scaling the images make them uglier and less accurate?
hi * Will being able to see more images be work the detail loss?
hi * Are there people using this on 4k moniters that can fit like 20000 images at once?
hi * Will my comments become longer than my program itself?
hi * Will people make fun of how bad my code is?
hi * Will people make fun of how bad my comments are?
hi *
hi * These are the things that keep me awake at night
hi */
int total[3] = {0, 0, 0};
int avg[3];
int minRed = 255;
int maxRed = 0;

int minBlue = 255;
int maxBlue = 0;

int minGreen = 255;
int maxGreen = 0;
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
		// (2r - g)/2 works pretty well, but misses some buoys on very light images
		// (2r - 2g/3)/2 find more buoys, but also gets white blobs in the background (see 14_07_26_01_49_35 in 2014 comp logs)
		// blue probably has to be factored in somehow to get a more accurate value
		int outRed = 0;//(float) 1 * r / (g+1) * std::max((2*r - 1*g - 0*b) / 2 + 30, 0);
//		outRed = 0;
		// get the max and min values of the image for scaling in the next step
		if (outRed > maxRed)
		{
			maxRed = outRed;
		}
		if (outRed < minRed)
		{
			minRed = outRed;
		}

		// This one picks up only green buoys, but misses darker parts of the bouy (the bottom)\
		   and bouys that are futher away (lower red and green values)
		int outGreen = (int) std::max(50 + 4*g - 3*b - r - std::max(r, b), 0);

		if (outGreen > maxGreen)
		{
			maxGreen = outGreen;
		}
		if (outGreen < minGreen)
		{
			minGreen = outGreen;
		}

		// who knows what this does
		// probably yellow buoys eventually
		int outBlue = 0;

		if (outBlue > maxBlue)
		{
			maxBlue = outBlue;
		}
		if (outBlue < minBlue)
		{
			minBlue = outBlue;
		}

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
hi * This stream has a lot of setup time
hi * It should run more efficiently
hi * My stomach is starting to hurt now
hi */

/*
 * Second step of processing
 * subtract average and scale to 255
 */
cv::Mat thresholdImage(cv::Mat input)
{
	total[0] = 1;
	total[1] = 1;
	total[2] = 1;
	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();

	for (int i = 0; i< input.rows * input.cols; i++)
	{
		int inBlue = inPtr[3*i];
		int inGreen = inPtr[3*i + 1];
		int inRed = inPtr[3*i + 2];

		// remove average
		int outBlue = std::max(inBlue - avg[0], 0);
		int outGreen = std::max(inGreen - avg[1], 0);
		int outRed = std::max(inRed - avg[2], 0);

		// scale values so that highest is 255
		outBlue = outBlue * 255 / (maxBlue + 1);
		outGreen = outGreen * 255 / (maxGreen + 1);
		outRed = outRed * 255 / (maxRed + 1);

		// get the new total red, green, and blue values to find a weighed average in the next step
		total[0] += outBlue;
		total[1] += outGreen;
		total[2] += outRed;

		outPtr[3*i] = outBlue;
		outPtr[3*i + 1] = outGreen;
		outPtr[3*i + 2] = outRed;
	}
	return output;
}

/*
 * Last step of processing
 * apply power function to filter out noise
 */
cv::Mat lastStep(cv::Mat input)
{
	cv::Mat output(input.rows, input.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	unsigned char* inPtr = input.ptr();
	unsigned char* outPtr = output.ptr();

	total[0] = 1;
	total[1] = 1;
	total[2] = 1;
	long totalRed[2] = {0, 0};
	long totalGreen[2] = {0, 0};
	long totalBlue[2] = {0, 0};
	for (int i = 0; i< input.rows * input.cols; i++)
	{
		int inBlue = inPtr[3*i];
		int inGreen = inPtr[3*i + 1];
		int inRed = inPtr[3*i + 2];
		int outBlue = inBlue;
		int outGreen = inGreen;
		int outRed = inRed;

		// exponent (for displayed image)
		outBlue = (long)pow(outBlue, 3) / (long)pow(255, 2);
		outGreen = (long)pow(outGreen, 3) / (long)pow(255, 2);
		outRed = (long)pow(outRed, 3) / (long)pow(255, 2);

		// get the weighed total of the x and y coordinates of each color
		// \
hi		luke, does "\" cancel out the newline and keep comments on the next?\
hi		this is pretty cool
		
		outPtr[3*i] = outBlue;
		outPtr[3*i + 1] = outGreen;
		outPtr[3*i + 2] = outRed;
		
		// exponent (only for com, not for displayed image)
		outBlue = (long)pow(outBlue, 4) / (long)pow(255, 3);
		outGreen = (long)pow(outGreen, 4) / (long)pow(255, 3);
		outRed = (long)pow(outRed, 4) / (long)pow(255, 3);

		totalRed[0] += (i % input.cols) * outRed;
		totalRed[1] += (i / input.cols) * outRed;

		totalGreen[0] += (i % input.cols) * outGreen;
		totalGreen[1] += (i / input.cols) * outGreen;

		totalBlue[0] += (i % input.cols) * outBlue;
		totalBlue[1] += (i / input.cols) * outBlue;

		total[0] += outBlue;
		total[1] += outGreen;
		total[2] += outRed;
	}
	// find the centroid of the image
	// this results in the path messing up the position if it is in the image
	// but it can probably be processed out somehow
	// (maybe removing points below the centroid if standard deviation is lage enough?)
	int centRed[2] = {totalRed[0] / total[2], totalRed[1] / total[2]};
	int centGreen[2] = { totalGreen[0] / total[1],  totalGreen[1] / total[1]};
//	int centBlue[2] = {totalBlue[0] / total[0], totalBlue[1] / total[0]};

	// just use the blue channel to make a 10x10 square at the center
	// because the blue is stupid and useless right now \
hi
	for (int i = centRed[0] - 5; i < centRed[0] + 5; i++)
	{
		for (int j = centRed[1] - 5; j < centRed[1] + 5; j++)
		{
			if ((i > 0) && (i < input.cols) && (j > 0) && (j < input.rows))
			{
				outPtr[3*(i + j * input.cols)] = 255;
			}
		}
	}
	for (int i = centGreen[0] - 5; i < centGreen[0] + 5; i++)
	{
		for (int j = centGreen[1] - 5; j < centGreen[1] + 5; j++)
		{
			if ((i > 0) && (i < input.cols) && (j > 0) && (j < input.rows))
			{
				outPtr[3*(i + j * input.cols)] = 255;
			}
		}
	}
	return output;
}

cv::Mat filterImage(cv::Mat input, cv::Mat filter)
{
	long total = 1;
	for (int i = 0; i < filter.rows * filter.cols * 3; i++)
	{
		total += filter.ptr()[i];
	}
	int avg = total / (filter.rows * filter.cols * 3);
	for (int i = 0; i < input.rows * input.cols * 3; i++)
	{
		input.ptr()[i] = std::max((int)input.ptr()[i] - filter.ptr()[i] + avg, 0);
	}
	return input;
}

int main (int argc, char* argv[])
{
	cv::Mat filter = cv::imread("FILTER.jpg", CV_LOAD_IMAGE_COLOR);
	for (int i = 0; i < argc - 1; i++)
	{
		cv::Mat inImg;
		cv::Mat transImg;
		cv::Mat thresImg;
		cv::Mat outImg;

		// get the image, run and display the result of all 3 steps of processing on the image
		// \
hi		The parallelism on that sentence goes (run) and (display the result of) all...\
hi		Not (run) and (display) the result of all... \
hi		I'm not sure if that's gramatically corret, but it's what I wrote
		inImg = cv::imread(argv[i + 1], CV_LOAD_IMAGE_COLOR);
		if (inImg.rows == 0 || inImg.cols == 0) continue; // ignore nonexistent image
		transImg = transformImage(inImg);
		thresImg = thresholdImage(transImg);
		outImg = lastStep(thresImg);
		std::cout<<total<<" "<<total<<"\n";
		showImages(inImg, transImg, thresImg, outImg);
	}
	
}

/*
hi * P.S.
hi * My mouth doesn't burn anymore
hi * Also, Luke, I want to try the browser you use, but I keep forgetting the letters
hi * You should reply to this with the name of the browser.
hi * I tried pressing random keys but I just got a bunch of letters on the home row.
hi * like this: ldkfah
hi * None of the results from that search looked like browsers.
hi * 
hi * P.P.S.
hi * I only stopped commenting because Apex ended for today.
hi * Who knows how much longer I could have gone.
hi * Maybe I could have broken 400 lines
hi *
hi * P.P.P.S.
hi * All of the useless comments have lines beginning with hi
hi * You can use regular expressions to find and delete them
hi *
hi * P.P.P.P.S.
hi * The first line of multiline comments doesn't have a hi
hi * because then I would have a hi outside of a comment.
hi * Also, I might have missed some when I was putting hi at the beginning.
hi * You should read the comments to make sure.
hi * Or I will next week.
hi * 
hi * P.P.P.P.P.S.
hi * If you are too lazy to make a regex yourself,
hi * This vim command thing gets rid of all of the comments that I marked
hi * 	:%s/^.\+\n\(^\hi.*\n\)\+//g
hi * Don't make fun of me for being bad at regexes
hi * I made this with a stomachache. Also I have no idea how regex works
hi * 
hi * P.P.P.P.P.P.S.
hi * Oh dang you emailed me and asked me for this
hi * I should check my email at night before I sleep
hi */
