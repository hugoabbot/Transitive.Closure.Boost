.PHONY: all clean

CXX := g++
CXXFLAGS := -std=c++17

TARGET := tc.out
SOURCE := tc.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) -O3 $(SOURCE)

clean:
	rm -f $(TARGET)  
