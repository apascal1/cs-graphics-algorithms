# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall -Dcimg_display=0
TARGET = seamcarving
SRC = seamcarving.cpp
OBJS = $(SRC:.cpp=.o)
LIBS = -lm

# Default parameters (can be overridden) 
input ?= image.png
output ?= result.jpg
width ?= 300
height ?= 200

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run with default or custom parameters 
run: $(TARGET)
	./$(TARGET) $(input) $(output) $(width) $(height)

clean:
	rm -f $(TARGET) $(OBJS) *.jpg *.png *.out
