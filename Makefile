CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude

SRC_DIR   := src
CLI_DIR   := cli
BENCH_DIR := bench

# Source files
SRCS := $(SRC_DIR)/Scheduler.cpp \
        $(SRC_DIR)/FCFS.cpp \
        $(SRC_DIR)/SJF.cpp \
        $(SRC_DIR)/RoundRobin.cpp \
        $(SRC_DIR)/Priority.cpp

# Main CLI tool
scheduler: $(SRCS) $(CLI_DIR)/Renderer.cpp $(CLI_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✓ Built: ./scheduler"

# Heap vs Naive benchmark
benchmark: $(SRCS) $(BENCH_DIR)/Benchmark.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✓ Built: ./benchmark"

all: scheduler benchmark

clean:
	rm -f scheduler benchmark

.PHONY: all clean
