CXX := g++
CXXFLAGS := -std=c++17 -g
TARGET := solution
SRC := solution.cpp

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o game_output.csv

test: $(TARGET)
	pytest test_game.py