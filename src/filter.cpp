#include <iostream>
//#include<fstream>
#include <unordered_set>
#include <string>

int main(){
	//std::ifstream wall ("wall", std::ifstream::in);
	std::unordered_set<std::string> values;
	//std::string next_line;
	/*
	while( std::getline(wall, next_line) ) 
	{
		values.insert(next_line);
		
	}
	*/
	std::string line;
	while (std::getline(std::cin, line))
	{
		values.insert(line);
	}
	
	for(auto i = values.begin(); i != values.end(); i++)
	{
		std::cout << *i << std::endl;
	}
	return 0;

}
