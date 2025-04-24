# Multithreading Performance and Execution Time Analysis

## Introduction

This was one of the first times I seriously explored what happens under the hood of a program once you introduce multithreading. I wasn't content just writing code that worked. I wanted to measure, test, and understand how different algorithms behave when scaled across multiple threads. From sorting to matrix math to searches, this project is all about performance under pressure.

I originally built this as a school project, but pushed it way beyond the minimum just to see what was really possible.

## What It Does

The program benchmarks various algorithms, including sorting, matrix operations, and searching, by running them under different thread and data size configurations. It tracks execution time using `clock_gettime()` and helps visualize how performance scales (or doesn't) when you start throwing more threads at the problem.

## Key Features

Multi-threaded execution with dynamic thread and data size configuration\
Benchmarks for both single-threaded and multi-threaded execution\
Per-thread and global time tracking using `clock_gettime()`\
Thread-safe execution via mutexes\
Clean CLI interface for easy config and execution

## Implemented Algorithms

### Sorting

Bubble Sort\
Quick Sort\
Merge Sort\
Insertion Sort\
Selection Sort\
Heap Sort

### Matrix Operations

Multiplication\
Addition\
Transposition

### Search Algorithms

Linear Search\
Binary Search

## How to Use It

```bash
# Run an algorithm with thread and data size ranges
./program run <threads_start> <threads_end> <data_size_start> <data_size_end>

# Measure how performance scales with thread count
./program test <threads_start> <threads_end>

# Analyze one specific algorithm
./program analyze

# Toggle verbose output
./program verbose <true/false>

# Show help
./program help
```

## Takeaways

Multithreading can seriously speed things up if done right.\
Sorting and matrix operations scale well. Searching is trickier due to uneven work distribution.\
Thread overhead becomes real past a certain point, especially if you're not careful with data splits and sync.

## License

MIT. Fork it, break it, improve it.

