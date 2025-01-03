#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include "Algorithm.cpp"

bool verbose = false;
bool iterative = false;
bool jsonOutput = false;
int testSize = 1;
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
        LINEAR_SEARCH,
        BINARY_SEARCH,
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
    if (algorithm == "linear_search") {
        return AlgorithmType::LINEAR_SEARCH;
    }
    if (algorithm == "binary_search") {
        return AlgorithmType::BINARY_SEARCH;
    }
    std::cerr << "Unknown algorithm '" << algorithm << "'.\n";
    return AlgorithmType::UNKNOWN;
}

Measurement selectAlgorithm(const std::string& algorithm, int threadCount, long long dataSize) {
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
        case AlgorithmType::MATRIX_MULTIPLICATION:
            algo = new MatrixMultiplication(threadCount, dataSize, verbose);
            break;
        case AlgorithmType::MATRIX_ADDITION:
            algo = new MatrixAddition(threadCount, dataSize, verbose);
            break;
        case AlgorithmType::MATRIX_TRANSPOSE:
            algo = new MatrixTransposition(threadCount, dataSize, verbose);
            break;
        case AlgorithmType::LINEAR_SEARCH:
            algo = new LinearSearch(threadCount, dataSize, verbose);
            break;
        case AlgorithmType::BINARY_SEARCH:
            algo = new BinarySearch(threadCount, dataSize, verbose);
            break;
        default:
            std::cerr << "Error: Unsupported or unknown algorithm '" << algorithm << "'.\n";
            return Measurement();
    }

    Measurement result = algo->executeAndMeasure(threadCount, dataSize);
    if (!jsonOutput) {
        std::cout << result.toString() << std::endl;
    }
    delete algo;
    return result;


}


void runAlgorithm(const std::string& algorithm, int fireStart, int fireEnd, int sizeStart, int sizeEnd) {
    // limit the size of the data to LLONG_MAX
    sizeStart = std::ranges::min(sizeStart, 63);
    sizeStart = std::ranges::max(sizeStart, 0);
    sizeEnd = std::ranges::max(sizeEnd, 0);
    sizeEnd = std::ranges::min(sizeEnd, 63);
    // swap if size start is greater than size end
    if (sizeStart > sizeEnd) {
        std::swap(sizeStart, sizeEnd);
    }
    fireStart = std::ranges::max(fireStart, 0);
    fireEnd = std::ranges::max(fireEnd, 0);
    fireStart = std::ranges::min(fireStart, 31);
    fireEnd = std::ranges::min(fireEnd, 31);
    // swap if fire start is greater than fire end
    if (fireStart > fireEnd) {
        std::swap(fireStart, fireEnd);
    }
    if (!jsonOutput) {
        std::cout << "Running " << algorithm << " with varying threads and data sizes...\n";
    }
    nlohmann::json allResults = nlohmann::json::array();

    for (int i = sizeStart; i <= sizeEnd; ++i) {
        auto dataSize = static_cast<long long>(pow(2, i));
        for (int j = fireStart; j <= fireEnd; ++j) {
            int numThreads = static_cast<int>(pow(2, j));
            nlohmann::json jsonObject;
            jsonObject["algorithm"] = algorithm;
            jsonObject["threads"] = numThreads;
            jsonObject["data_size"] = dataSize;

            // Call selectAlgorithm and capture its output if necessary, also redo the measurements testSize times
            // and return the average
            int successFullTests = 0;
            Measurement finalResult = selectAlgorithm(algorithm, numThreads, dataSize);
            for (int k = 0; k < testSize-1; ++k) {
                Measurement result = selectAlgorithm(algorithm, numThreads, dataSize);
                if (result == Measurement()) {
                    continue;
                }
                finalResult += result;
                successFullTests++;
            }
            finalResult.duration = finalResult.duration / testSize;
            // Measurement result = selectAlgorithm(algorithm, numThreads, dataSize);
            // if (result == Measurement()) {
            //     continue;
            // }
            jsonObject["test_count"] = successFullTests + 1;
            jsonObject["result"] = finalResult.toJson();

            // Accumulate all results in a single array
            allResults.push_back(jsonObject);
        }
    }

    // Output the entire JSON structure at the end
    if (jsonOutput) {
        std::cout << allResults.dump(4) << std::endl;
    } else {
        for (const auto& result : allResults) {
            std::cout << "Algorithm: " << result["algorithm"]
                      << ", Threads: " << result["threads"]
                      << ", Data size: " << result["data_size"] << std::endl;
        }
    }
}

void testAlgorithm(const std::string& algorithm, int fireStart, int fireEnd) {
    if (!jsonOutput)
    {
        std::cout << "Testing " << algorithm << " with varying threads and data sizes...\n";
    }
    runAlgorithm(algorithm, fireStart, fireEnd, 5, 10);
}

void analyzeAlgorithm(const std::string& algorithm) {
    if (!jsonOutput) {
        std::cout << "Analyzing performance for " << algorithm << " with various configurations...\n";
    }
    testAlgorithm(algorithm, 0, 4);
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
    return value >= 0;
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
    } else if (command == "json_output") {
        std::string state;
        if (ss >> state) {
            if (state == "true") {
                jsonOutput = true;
                std::cout << "JSON output enabled.\n";
            } else if (state == "false") {
                jsonOutput = false;
                std::cout << "JSON output disabled.\n";
            } else {
                std::cerr << "Error: Invalid parameter for 'json_output' command. Use 'true' or 'false'.\n";
            }
        } else {
            std::cerr << "Error: Missing parameter for 'json_output' command.\n";
        }
    }
    else {
        std::cerr << "Error: Unknown command.\n";
    }
}

int main_infinite() {
    processCommand("verbose true");
    processCommand("json_output false");
    processCommand("analyze heap_sort");
    std::string input;
    if (!jsonOutput) {
        std::cout << "Enter a command (type 'help' for instructions):\n";
    }
    while (std::getline(std::cin, input)) {
        processCommand(input);
        if (!jsonOutput) {
            std::cout << "Enter a command (type 'help' for instructions):\n";
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string commandLine;
        for (int i = 1; i < argc; ++i) {
            commandLine += argv[i];
            if (i < argc - 1) {
                commandLine += " ";
            }
            if (argv[i] == "--use-iterative") {
                iterative = true;
            }
            // --repeat=INT is used to repeat the test INT times
            if (std::string(argv[i]).find("--repeat=") != std::string::npos) {
                std::string repeat = std::string(argv[i]).substr(9);
                testSize = std::stoi(repeat);
            }
        }
        jsonOutput = true;
        // check at the end if the command is iterative by checking if the last word is --use-iterative
        if (commandLine.find("--use-iterative") != std::string::npos) {
            iterative = true;
        }
        processCommand(commandLine);
    } else {
        main_infinite();
    }
    return 0;
}