#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <stdio.h>
#include <cmath>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <string>
using namespace std;

int main( int argc, char** argv )
{
	if(argc<6){
		std::cout<<"format: "<<argv[0]<<" [img folder] [buoy point file] [rectangle width] [rectangle height] [shift]"<<std::endl;
		return 0;
	} 
	string folder = argv[1];
	std::ifstream infile(argv[2]);
	int width = std::atoi(argv[3]);
	int height = std::atoi(argv[4]);
	int shift = std::atoi(argv[5]);

	int a, b;
	string filename;
	while (infile >> a >> b >> filename)
	{
		int num=0;
		int num2=0;
		cv::Mat src = cv::imread( folder+"/"+filename, 1 );
		for(int x=0;x<src.cols-width;x+=shift)
		{
			for(int y=0;y<src.rows-height;y+=shift)
			{
				int tlx = a-width/2;
				int tly = b-height/2;
				if(std::abs(x-tlx)<shift && std::abs(y-tly)<shift){
					//image is good
					cv::Rect myROI(x, y, width, height);
					cv::Mat cropped = src(myROI);
					string outfile = string("good/")+std::to_string(num2)+string("_")+filename;
//					string outfile = std::to_string(num)+string("_")+filename;
					imwrite(outfile, cropped );
					std::cout<< outfile << std::endl;
					num2++;
				} else {
					//image is bad
					if(std::abs((x+width/2)-a)>width && std::abs((y+height/2)-b)>height){
						cv::Rect myROI(x, y, width, height);
						cv::Mat cropped = src(myROI);
						string outfile = string("bad/")+std::to_string(num)+string("_")+filename;
	//					string outfile = std::to_string(num)+string("_")+filename;
						imwrite(outfile, cropped );
						std::cout<< outfile << std::endl;
						num++;
					}
				}
			}
		}
	}
/*
	*/
	return(0);
}
