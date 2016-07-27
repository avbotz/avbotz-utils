#include <iostream>
#include <string>
#include<math.h>
#include<vector>
#include<stdlib.h>

enum {RED, GREEN, BLUE};
//Color holds all the color values we read. I could use opencv for this but tbh it's not tha readable and I don't need the whole library for one cv::Scalar of colors
struct Color {
	int red, green, blue;
		 
	int index;  
	void find_index(){
		index = red + (256 * green) + (256 * 256 * blue);
	}

	int number;
};

//holds histogram range data 

//global variables, ew.
//good function structure? too lazy
const int hist_number = 16777216;
Color ranges[hist_number];
std::vector<int> indices;

int main(int argc, char** argv){
 
	int reduction = atoi(argv[1]);
 
	Color buf;
	long total_repetitions;
 
	while (scanf(" [%i,%i,%i],", &buf.red, &buf.green, &buf.blue) != EOF) {
//std::cin >> buf.red >> buf.green >> buf.blue) {
		buf.find_index();
		std::cerr << "read...\n";
		if(!ranges[buf.index].number){
			buf.number = 1;
			ranges[buf.index] = buf;
			indices.push_back(buf.index);
		}
		else {
			ranges[buf.index].number++;
		}
	} 
	 
	std::cerr << "done reading : " << std::endl;
	std::cerr << "indices: " << indices.size() << std::endl;	
	 
	for(int i = 0; i < indices.size(); i++){
		//std::cerr << "cycling through indices.\n";
		int it = indices[i];
		int final_number = (int)(ranges[it].number / reduction) + 1;
		total_repetitions += final_number;
		for(int l = 0; l < final_number; l++){
			std::cout << "[" << ranges[it].red << ","  << ranges[it].green << "," <<   ranges[it].blue << "],\n";
		}
	}
 
	float avg_repetitions = (float) total_repetitions / indices.size();
	std::cerr << "avg repetitions: " << avg_repetitions << std::endl;
	return 0;
 

}
