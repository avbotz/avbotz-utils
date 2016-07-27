all: threshold pixpik filter hist

threshold:
	g++ -g src/threshold.cpp -I/usr/include/opencv -lopencv_core -lopencv_highgui -o threshold

pixpik:
	g++ -ggdb -std=c++11 src/pixpik.cpp -Iinclude `pkg-config --cflags opencv` `pkg-config --libs opencv` -o pixpik

filter:
	g++ -ggdb -std=c++11 src/filter.cpp -o filter

hist:
	g++ -ggdb -std=c++11 src/hist.cpp -o hist

clean:
	rm threshold pixpik filter hist
