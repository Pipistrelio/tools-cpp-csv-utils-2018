CXX ?= g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic
LDFLAGS =

SRC = src/main.cpp src/normalize.cpp
HDR = src/csv.h src/normalize.h
BIN = csvutil

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	rm -f $(BIN)
