all: main

main: main.cpp
	g++ main.cpp -o main `pkg-config --cflags --libs opencv`
clean:
	rm main
