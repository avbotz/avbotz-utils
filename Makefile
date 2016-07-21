all: threshold pixpik 

threshold:
	g++ -g src/threshold.cpp -I/usr/include/opencv -lopencv_core -lopencv_highgui -o threshold

pixpik:
	g++ -ggdb -std=c++11 src/pixpik.cpp -Iinclude `pkg-config --cflags opencv` `pkg-config --libs opencv` -o pixpik

clean:
	rm threshold pixpik
