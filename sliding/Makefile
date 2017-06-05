CC = gcc
CXX = g++

LIBS = opencv

LDFLAGS = -g $(shell pkg-config --libs $(LIBS))


CPPFLAGS =
CFLAGS = -c -std=c11 -Wall -Wpedantic -Wextra $(shell pkg-config --cflags $(LIBS))
CXXFLAGS = -c -std=c++11 -Wall -Wextra -g $(shell pkg-config --cflags $(LIBS))

BUILDDIR = build
SRCDIR = src

SOURCES = $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*.cpp)
OBJECTS = $(addsuffix .o, $(notdir $(basename $(SOURCES))))

TARGET = slide

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(addprefix $(BUILDDIR)/, $(OBJECTS)) -o $@ $(LDFLAGS)

%.o: $(SRCDIR)/%.c
	$(CC) $< -o $(BUILDDIR)/$@ $(CFLAGS) $(CPPFLAGS)

%.o: $(SRCDIR)/%.cpp
	$(CXX) $< -o $(BUILDDIR)/$@ $(CXXFLAGS) $(CPPFLAGS)
