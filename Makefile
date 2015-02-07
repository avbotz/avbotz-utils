all: threshold

threshold: threshold.cpp
	g++ -g threshold.cpp -I/usr/include/opencv -lopencv_core -lopencv_highgui -o threshold

clean:
	rm threshold
