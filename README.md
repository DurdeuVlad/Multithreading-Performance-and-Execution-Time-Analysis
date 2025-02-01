Multithreading Performance Analysis & Execution Time Measurement
Overview
This project explores the impact of multithreading on execution time and CPU efficiency by analyzing various sorting, matrix operations, and search algorithms. Developed in C, with performance optimizations using assembly language, the program measures execution time across multiple threads and evaluates performance scaling.

Features
🔹 Multi-threaded execution with dynamic configuration of thread count and data size
🔹 Performance benchmarking for various algorithms in single-threaded vs. multi-threaded environments
🔹 Fine-grained time measurement using clock_gettime() for global and per-thread execution time
🔹 Synchronization mechanisms using mutexes to prevent race conditions
🔹 Command-line interface (CLI) for easy execution and analysis
Implemented Algorithms
📌 Sorting
Bubble Sort, Quick Sort, Merge Sort, Insertion Sort, Selection Sort, Heap Sort
📌 Matrix Operations
Matrix multiplication, addition, transposition, Strassen's algorithm
📌 Search Algorithms
Linear Search, Binary Search
Technology Stack
Programming Languages: C, Assembly
Concurrency & Threads: POSIX Threads (pthread)
Performance Measurement: clock_gettime() for execution time tracking
Synchronization: Mutexes to prevent race conditions
Command-line Interface: User interaction for configuring algorithm execution
Usage
bash
Copy code
# Run an algorithm with specified thread count and data size
./program run <threads_start> <threads_end> <data_size_start> <data_size_end>

# Test performance for different thread counts
./program test <threads_start> <threads_end>

# Analyze execution performance of a selected algorithm
./program analyze 

# Enable or disable verbose mode
./program verbose <true/false>

# Display available commands and usage instructions
./program help
Results & Insights
🏎 Multithreading significantly improves performance, but excessive threads introduce context-switching overhead
📊 Sorting algorithms benefit from thread-based parallelism, while search algorithms require adaptive workload distribution
🛠 Assembly optimizations reduced execution time by up to 30% compared to standard C implementations
