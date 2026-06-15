CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall
TARGET = teapot
SRC = teapot.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET) teapot-3.nff teapot-3.ppm

clean:
	rm -f $(TARGET) *.o *.ppm
