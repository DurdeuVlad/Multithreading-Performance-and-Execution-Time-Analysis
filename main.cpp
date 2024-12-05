#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include "Algorithm.cpp"

bool verbose = false;
bool iterative = false;
struct AlgorithmType {
    enum Type {
        BUBBLE_SORT,
        QUICK_SORT,
        MERGE_SORT,
        INSERTION_SORT,
        SELECTION_SORT,
        HEAP_SORT,
        MATRIX_MULTIPLICATION,
        MATRIX_ADDITION,
        MATRIX_TRANSPOSE,
        UNKNOWN
    };
};

AlgorithmType::Type getAlgorithmType(const std::string &algorithm) {
    if (algorithm == "bubble_sort") {
        return AlgorithmType::BUBBLE_SORT;
    }
    if (algorithm == "quick_sort") {
        return AlgorithmType::QUICK_SORT;
    }
    if (algorithm == "merge_sort") {
        return AlgorithmType::MERGE_SORT;
    }
    if (algorithm == "insertion_sort") {
        return AlgorithmType::INSERTION_SORT;
    }
    if (algorithm == "selection_sort") {
        return AlgorithmType::SELECTION_SORT;
    }
    if (algorithm == "heap_sort") {
        return AlgorithmType::HEAP_SORT;
    }
    if (algorithm == "matrix_multiplication") {
        return AlgorithmType::MATRIX_MULTIPLICATION;
    }
    if (algorithm == "matrix_addition") {
        return AlgorithmType::MATRIX_ADDITION;
    }
    if (algorithm == "matrix_transpose") {
        return AlgorithmType::MATRIX_TRANSPOSE;
    }
    std::cerr << "Unknown algorithm '" << algorithm << "'.\n";
    return AlgorithmType::UNKNOWN;
}

void selectAlgorithm(const std::string& algorithm, int threadCount, long long dataSize) {
    Algorithm* algo = nullptr;

    AlgorithmType::Type type = getAlgorithmType(algorithm);

    switch (type) {
        case AlgorithmType::BUBBLE_SORT:
            algo = new BubbleSort(threadCount, dataSize, verbose, &iterative);
            break;
        case AlgorithmType::QUICK_SORT:
            algo = new QuickSort(threadCount, dataSize, verbose, &iterative);
            break;
        case AlgorithmType::MERGE_SORT:
            algo = new MergeSort(threadCount, dataSize, verbose, &iterative);
            break;
        case AlgorithmType::INSERTION_SORT:
            algo = new InsertionSort(threadCount, dataSize, verbose, &iterative);
            break;
        case AlgorithmType::SELECTION_SORT:
            algo = new SelectionSort(threadCount, dataSize, verbose, &iterative);
            break;
        case AlgorithmType::HEAP_SORT:
            algo = new HeapSort(threadCount, dataSize, verbose, &iterative);
            break;
        // case AlgorithmType::MATRIX_MULTIPLICATION:
        //     algo = new MatrixMultiplication(threadCount, dataSize, verbose, &iterative);
        //     break;
        // case AlgorithmType::MATRIX_ADDITION:
        //     algo = new MatrixAddition(threadCount, dataSize, verbose, &iterative);
        //     break;
        // case AlgorithmType::MATRIX_TRANSPOSE:
        //     algo = new MatrixTransposition(threadCount, dataSize, verbose, &iterative);
        //     break;
        default:
            std::cerr << "Error: Unsupported or unknown algorithm '" << algorithm << "'.\n";
        return;
    }

    Measurement result = algo->executeAndMeasure(threadCount, dataSize);
    std::cout << result.toString() << std::endl;

    delete algo;
}


void runAlgorithm(const std::string& algorithm, int fireStart, int fireEnd, int sizeStart, int sizeEnd) {
    std::cout << "Running " << algorithm << " with varying threads and data sizes...\n";
    for (int i = sizeStart; i <= sizeEnd; ++i) {
        long long dataSize = pow(2, i);
        for (int j = fireStart; j <= fireEnd; ++j) {
            int numThreads = pow(2, j);
            std::cout << "Algorithm: " << algorithm
                      << ", Threads: " << numThreads
                      << ", Data size: " << dataSize << std::endl;
            selectAlgorithm(algorithm, numThreads, dataSize);
        }
    }
}

void testAlgorithm(const std::string& algorithm, int fireStart, int fireEnd) {
    std::cout << "Testing " << algorithm << " with varying threads and data sizes...\n";
    runAlgorithm(algorithm, fireStart, fireEnd, 10, 20);
}

void analyzeAlgorithm(const std::string& algorithm) {
    std::cout << "Analyzing performance for " << algorithm << " with various configurations...\n";
    testAlgorithm(algorithm, 1, 4);
}

void toggleVerbose(bool enableVerbose) {
    verbose = enableVerbose;
    std::cout << "Verbose mode " << (verbose ? "enabled" : "disabled") << ".\n";
}

void showHelp() {
    std::cout << "Available commands:\n";
    std::cout << "run <algorithm> <fire_start> <fire_end> <data_size_start> <data_size_end>\n";
    std::cout << "test <algorithm> <fire_start> <fire_end>\n";
    std::cout << "analyze <algorithm>\n";
    std::cout << "verbose <true/false>\n";
    std::cout << "help\n";
}

bool isValidPositive(int value) {
    return value > 0;
}

void processCommand(const std::string& commandLine) {
    std::istringstream ss(commandLine);
    std::string command;
    ss >> command;

    if (command == "run") {
        std::string algorithm;
        int fireStart, fireEnd, sizeStart, sizeEnd;
        if (ss >> algorithm >> fireStart >> fireEnd >> sizeStart >> sizeEnd) {
            if (isValidPositive(fireStart) && isValidPositive(fireEnd) &&
                isValidPositive(sizeStart) && isValidPositive(sizeEnd)) {
                runAlgorithm(algorithm, fireStart, fireEnd, sizeStart, sizeEnd);
            } else {
                std::cerr << "Error: Parameters for 'run' must be positive numbers.\n";
            }
        } else {
            std::cerr << "Error: Invalid parameters for 'run' command.\n";
        }
    } else if (command == "test") {
        std::string algorithm;
        int fireStart, fireEnd;
        if (ss >> algorithm >> fireStart >> fireEnd) {
            if (isValidPositive(fireStart) && isValidPositive(fireEnd)) {
                testAlgorithm(algorithm, fireStart, fireEnd);
            } else {
                std::cerr << "Error: Parameters for 'test' must be positive numbers.\n";
            }
        } else {
            std::cerr << "Error: Invalid parameters for 'test' command.\n";
        }
    } else if (command == "analyze") {
        std::string algorithm;
        if (ss >> algorithm) {
            analyzeAlgorithm(algorithm);
        } else {
            std::cerr << "Error: Invalid parameters for 'analyze' command.\n";
        }
    } else if (command == "verbose") {
        std::string state;
        if (ss >> state) {
            if (state == "true") {
                toggleVerbose(true);
            } else if (state == "false") {
                toggleVerbose(false);
            } else {
                std::cerr << "Error: Invalid parameter for 'verbose' command. Use 'true' or 'false'.\n";
            }
        } else {
            std::cerr << "Error: Missing parameter for 'verbose' command.\n";
        }

    }
    else if (command == "iterative") {
        std::string state;
        if (ss >> state) {
            if (state == "true") {
                iterative = true;
                std::cout << "Iterative mode enabled.\n";
            } else if (state == "false") {
                iterative = false;
                std::cout << "Iterative mode disabled.\n";
            } else {
                std::cerr << "Error: Invalid parameter for 'verbose' command. Use 'true' or 'false'.\n";
            }
        } else {
            std::cerr << "Error: Missing parameter for 'verbose' command.\n";
        }
    }
    else if (command == "help") {
        showHelp();
    } else {
        std::cerr << "Error: Unknown command.\n";
    }
}

int main() {
    std::string input;
    // processCommand("analyze matrix_multiplication");
    processCommand("verbose true");
    processCommand("analyze bubble_sort");
    // processCommand("analyze quick_sort");
    std::cout << "Enter a command (type 'help' for instructions):\n";
    while (std::getline(std::cin, input)) {
        processCommand(input);
        std::cout << "\nEnter another command (or 'help' for instructions):\n";
    }
    return 0;
}
