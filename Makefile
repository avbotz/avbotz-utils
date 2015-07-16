all: main

main: main.cpp
	g++ -std=c++11 main.cpp -o main `pkg-config --cflags --libs opencv`
clean:
	rm main
