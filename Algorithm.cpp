#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <functional>
#include <algorithm>
#include <queue>
#include <bits/stdc++.h>
#include <vector>
#include <algorithm> // For std::merge
#include <stdexcept> // For std::runtime_error

// ===================== Measurement =====================
class Measurement {
public:
    double threadCount, duration, dataSize;
    long long start, end;
    bool correct;
    bool* iterative;

    Measurement(double threadCount, double duration, double dataSize, long long start, long long end, bool correct, bool* iterative = nullptr)
        : threadCount(threadCount), duration(duration), dataSize(dataSize), start(start), end(end), correct(correct), iterative(iterative) {}

    [[nodiscard]] std::string toString() const {
        if (iterative != nullptr) {
            return "Thread Count: " + std::to_string(threadCount) +
                   ", Duration: " + std::to_string(duration) +
                   ", Data Size: " + std::to_string(dataSize) +
                   ", Is algorithm correct?: " + std::to_string(correct) +
                   ", Is iterative?: " + std::to_string(*iterative);
        }
        return "Thread Count: " + std::to_string(threadCount) +
               ", Duration: " + std::to_string(duration) +
               ", Data Size: " + std::to_string(dataSize) +
               ", Is algorithm correct?: " + std::to_string(correct);
    }
};

// ===================== Algorithm =====================
class Algorithm {
protected:
    int threadCount;
    long long dataSize;
    bool verbose = false;
    bool* reiterative;

public:
    Algorithm(int threadCount, long long dataSize, bool verbose, bool* reiterative = nullptr)
        : threadCount(threadCount), dataSize(dataSize), verbose(verbose), reiterative(reiterative) {}

    virtual ~Algorithm() = default;

    std::vector<int*> executeTask(const std::vector<long long>& areaOfResponsibility, const std::vector<int*>& data) {
        return execute(areaOfResponsibility, data);
    }

    virtual void cleanupData(std::vector<int*>& data, std::vector<std::vector<int*>>& result) const = 0;

    Measurement executeAndMeasure(int threads, long long dataSize) {
        if (threads <= 0 || dataSize <= 0) {
            throw std::invalid_argument("Threads and data size must be greater than zero.");
        }

        if (verbose) {
            std::cout << "Starting execution with " << threads << " threads and data size " << dataSize << "." << std::endl;
        }

        std::vector<int*> data = generateData(dataSize);
        std::vector<std::thread> threadPool;
        std::vector<std::vector<int*>> result(threads);

        auto start = std::chrono::high_resolution_clock::now();

        try {
            for (int i = 0; i < threads; ++i) {
                auto areaOfResponsibility = calculate_area_of_responsibility(i, threads, dataSize);

                if (verbose) {
                    std::cout << "Thread " << i << " responsible for rows [" << areaOfResponsibility[0] << ", "
                              << areaOfResponsibility[1] << "]." << std::endl;
                }

                threadPool.emplace_back([&, i, areaOfResponsibility]() {
                    result[i] = executeTask(areaOfResponsibility, data);

                    if (verbose) {
                        std::cout << "Thread " << i << " completed its task." << std::endl;
                    }
                });
            }

            for (auto& t : threadPool) {
                if (t.joinable()) {
                    t.join();
                }
            }

            auto final_result = concat_results(result, data, threads, dataSize);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;

            bool results_are_correct = test_result(data, final_result, dataSize);

            if (verbose) {
                std::cout << "Execution completed in " << duration.count() << " seconds. Results are "
                          << (results_are_correct ? "correct." : "incorrect.") << std::endl;
            }

            cleanupData(data, result);

            return Measurement(
                static_cast<double>(threads),
                duration.count(),
                static_cast<double>(dataSize),
                start.time_since_epoch().count(),
                end.time_since_epoch().count(),
                results_are_correct,
                reiterative
            );

        } catch (const std::exception& e) {
            std::cerr << "Exception during execution: " << e.what() << std::endl;

            cleanupData(data, result);

            throw;
        }
    }

    virtual std::string getType() const = 0;

protected:
    virtual std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData) = 0;
    virtual std::vector<int*> generateData(long long dataSize) = 0;
    virtual std::vector<long long> calculate_area_of_responsibility(int currentThread, int maxThreads, long long dataSize) = 0;
    virtual bool test_result(const std::vector<int*>& input_data, const std::vector<int*>& result, long long dataSize) = 0;
    virtual std::vector<int*> concat_results(const std::vector<std::vector<int*>>& partial_results, const std::vector<int*>& inputData, int thread_count, long long data_size) = 0;
};


// ===================== SortingAlgorithm =====================
class SortingAlgorithm : public Algorithm {
public:
    SortingAlgorithm(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : Algorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "SortingAlgorithm";
    }

protected:
    void cleanupData(std::vector<int*>& data, std::vector<std::vector<int*>>& result) const override {
        if (!data.empty() && data[0] != nullptr) {
            delete[] data[0]; // Deallocate the main array
            data[0] = nullptr;
        }

        for (auto& partial_result : result) {
            if (!partial_result.empty() && partial_result[0] != nullptr) {
                delete[] partial_result[0]; // Deallocate each partial result
                partial_result[0] = nullptr;
            }
        }

        result.clear();
        data.clear();
    }

    std::vector<int*> concat_results(const std::vector<std::vector<int*>>& partial_results, const std::vector<int*>& inputData, int thread_count, long long data_size) override {
        if (verbose) {
            std::cout << "Merging partial results from threads." << std::endl;
        }

        // Allocate space for the merged data
        auto* merged_data = new int[data_size];
        std::vector<long long> start(thread_count), end(thread_count), indexes(thread_count);

        for (int i = 0; i < thread_count; ++i) {
            auto area_of_responsibility = calculate_area_of_responsibility(i, thread_count, data_size);
            start[i] = area_of_responsibility[0];
            end[i] = area_of_responsibility[1];
            indexes[i] = start[i];
        }

        int merged_index = 0;
        while (true) {
            int min_value = INT_MAX;
            int min_index = -1;

            // Find the smallest element among the threads
            for (int i = 0; i < thread_count; ++i) {
                if (indexes[i] < end[i] && inputData[0][indexes[i]] < min_value) {
                    min_value = inputData[0][indexes[i]];
                    min_index = i;
                }
            }

            if (min_index == -1) break; // No more elements to merge
            merged_data[merged_index++] = min_value;
            indexes[min_index]++;
        }

        if (verbose) {
            std::cout << "Merged data successfully." << std::endl;
        }

        return {merged_data}; // Return the merged data
    }

    bool test_result(const std::vector<int*>& input_data, const std::vector<int*>& result, long long dataSize) override {
        const int* sorted_data = result[0]; // Assuming the sorted result is stored in the first element
        for (long long i = 1; i < dataSize; ++i) {
            if (sorted_data[i - 1] > sorted_data[i]) {
                std::cout << "Sorting failed at index " << i << ": " << sorted_data[i - 1] << " > " << sorted_data[i] << std::endl;
                return false;
            }
        }
        return true;
    }

    std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData) override {
        int* data = inputData[0]; // Access the data
        long long start = area_of_responsibility[0];
        long long end = area_of_responsibility[1];
        sortSegment(data, start, end);
        return {}; // Sorting is in-place; no need to return data here
    }

    std::vector<int*> generateData(long long dataSize) override {
        auto* data = new int[dataSize]; // Dynamically allocate array
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1000);
        for (long long i = 0; i < dataSize; ++i) {
            data[i] = dis(gen);
        }
        return {data}; // Return the generated data as a vector containing a pointer
    }

    std::vector<long long> calculate_area_of_responsibility(int currentThread, int maxThreads, long long dataSize) override {
        long long segmentSize = dataSize / maxThreads;
        return {currentThread * segmentSize, (currentThread == maxThreads - 1) ? dataSize : (currentThread + 1) * segmentSize};
    }

    virtual void sortSegment(int* data, long long start, long long end) = 0;
};


// ===================== Algorithms =====================
class BubbleSort : public SortingAlgorithm {
public:
    BubbleSort(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : SortingAlgorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "BubbleSort";
    }

protected:
    void sortSegment(int* data, long long start, long long end) override {
        for (long long i = start; i < end - 1; ++i) {
            for (long long j = start; j < end - 1 - (i - start); ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
};

class QuickSort : public SortingAlgorithm {
public:
    QuickSort(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : SortingAlgorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "QuickSort";
    }

protected:
    void sortSegment(int* data, long long start, long long end) override {
        std::sort(data + start, data + end);
    }
};


class MergeSort : public SortingAlgorithm {
public:
    MergeSort(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : SortingAlgorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "MergeSort";
    }

protected:
    void sortSegment(int* data, long long start, long long end) override {
        if (end - start <= 1) return;
        long long mid = start + (end - start) / 2;
        sortSegment(data, start, mid);
        sortSegment(data, mid, end);

        // Temporary array for merging
        std::vector<int> temp(end - start);
        std::merge(data + start, data + mid, data + mid, data + end, temp.begin());
        std::copy(temp.begin(), temp.end(), data + start);
    }
};

class InsertionSort : public SortingAlgorithm {
public:
    InsertionSort(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : SortingAlgorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "InsertionSort";
    }

protected:
    void sortSegment(int* data, long long start, long long end) override {
        for (long long i = start + 1; i < end; ++i) {
            int key = data[i];
            long long j = i - 1;
            while (j >= start && data[j] > key) {
                data[j + 1] = data[j];
                --j;
            }
            data[j + 1] = key;
        }
    }
};


class SelectionSort : public SortingAlgorithm {
public:
    SelectionSort(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : SortingAlgorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "SelectionSort";
    }

protected:
    void sortSegment(int* data, long long start, long long end) override {
        for (long long i = start; i < end - 1; ++i) {
            long long minIndex = i;
            for (long long j = i + 1; j < end; ++j) {
                if (data[j] < data[minIndex]) {
                    minIndex = j;
                }
            }
            std::swap(data[i], data[minIndex]);
        }
    }
};


class HeapSort : public SortingAlgorithm {
public:
    HeapSort(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : SortingAlgorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "HeapSort";
    }

protected:
    void sortSegment(int* data, long long start, long long end) override {
        // Build the heap
        for (long long i = (end - 1) / 2; i >= start; --i) {
            heapify(data, end, i);
        }
        // Extract elements from the heap
        for (long long i = end - 1; i > start; --i) {
            std::swap(data[start], data[i]);
            heapify(data, i, start);
        }
    }

    void heapify(int* data, long long n, long long i) {
        long long largest = i;
        long long left = 2 * i + 1;
        long long right = 2 * i + 2;

        if (left < n && data[left] > data[largest]) {
            largest = left;
        }
        if (right < n && data[right] > data[largest]) {
            largest = right;
        }
        if (largest != i) {
            std::swap(data[i], data[largest]);
            heapify(data, n, largest);
        }
    }
};

// // ===================== MatrixOperationAlgorithm =====================
// class MatrixOperationAlgorithm : public Algorithm {
// public:
//     MatrixOperationAlgorithm(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
//         : Algorithm(threadCount, dataSize, verbose, reiterative) {}
//
//     [[nodiscard]] std::string getType() const override {
//         return "MatrixOperationAlgorithm";
//     }
//
// protected:
//     bool verbose;
//     void cleanupData(void** data, void*** result, int threads) const override {
//         if (verbose) {
//             std::cout << "Cleaning up matrix data and result." << std::endl;
//         }
//         auto* matrixData = reinterpret_cast<std::vector<std::vector<int>>*>(data);
//         delete matrixData;
//
//
//         if (verbose) {
//             std::cout << "Matrix data and result cleaned up." << std::endl;
//         }
//     }
//     void** generateData(long long dataSize) override {
//         if (verbose) {
//             std::cout << "Generating data for matrix of size " << dataSize << "x" << dataSize << std::endl;
//         }
//
//         auto* matrix = new std::vector<std::vector<int>>(dataSize, std::vector<int>(dataSize));
//         std::random_device rd;
//         std::mt19937 gen(rd());
//         std::uniform_int_distribution<> dis(1, 100);
//         for (auto& row : *matrix) {
//             std::ranges::generate(row, [&]() { return dis(gen); });
//         }
//
//         if (verbose) {
//             std::cout << "Data generation complete." << std::endl;
//         }
//
//         return reinterpret_cast<void**>(matrix);
//     }
//
//     long long* calculate_area_of_responsibility(int currentThread, int maxThreads, long long dataSize) override {
//         if (maxThreads <= 0 || dataSize <= 0) {
//             throw std::invalid_argument("Thread count and data size must be greater than zero.");
//         }
//
//         auto* range = new long long[2];
//         long long segmentSize = dataSize / maxThreads;
//         range[0] = currentThread * segmentSize;
//         range[1] = (currentThread == maxThreads - 1) ? dataSize : (currentThread + 1) * segmentSize;
//
//         if (range[0] >= range[1]) {
//             throw std::logic_error("Invalid range calculated for thread responsibility.");
//         }
//
//         if (verbose) {
//             std::cout << "Thread " << currentThread << " responsible for rows [" << range[0] << ", " << range[1] - 1 << "]\n";
//         }
//
//         return range;
//     }
//
//     void** execute(long long* area_of_responsibility, void** inputData) override {
//         if (verbose) {
//             std::cout << "Executing computation for assigned rows." << std::endl;
//         }
//
//         auto* matrix = reinterpret_cast<std::vector<std::vector<int>>*>(inputData);
//         auto* partialResult = new std::vector<std::vector<int>>();
//
//         for (long long row = area_of_responsibility[0]; row < area_of_responsibility[1]; ++row) {
//             partialResult->push_back(processRow((*matrix)[row], *matrix));
//         }
//
//         if (verbose) {
//             std::cout << "Execution complete for assigned rows." << std::endl;
//         }
//
//         return reinterpret_cast<void**>(partialResult);
//     }
//
//     bool test_result(void** input_data, void** result, long long dataSize) override {
//         auto* originalMatrix = reinterpret_cast<std::vector<std::vector<int>>*>(input_data);
//         auto* threadedResult = reinterpret_cast<std::vector<std::vector<int>>*>(result);
//         auto singleThreadResult = *originalMatrix;
//
//         if (verbose) {
//             std::cout << "Starting single-threaded execution for verification." << std::endl;
//         }
//
//         for (long long i = 0; i < dataSize; ++i) {
//             // if (verbose) {
//             //     std::cout << "Processing row " << i << " for single-threaded verification." << std::endl;
//             // }
//             singleThreadResult[i] = processRow(singleThreadResult[i], singleThreadResult);
//         }
//         if (verbose) {
//             std::cout << "Single-threaded execution complete." << std::endl;
//         }
//
//         if (singleThreadResult.size() != threadedResult->size()) {
//             throw std::runtime_error("Mismatch in matrix dimensions: single-threaded result and threaded result sizes differ.");
//         }
//
//         for (long long i = 0; i < dataSize; ++i) {
//             const auto& expectedRow = singleThreadResult[i];
//             const auto& actualRow = (*threadedResult)[i];
//
//             if (expectedRow.size() != actualRow.size()) {
//                 throw std::runtime_error("Row size mismatch at row " + std::to_string(i));
//             }
//
//             if (expectedRow != actualRow) {
//                 if (verbose) {
//                     std::cout << "Mismatch found in row " << i << std::endl;
//                     for (size_t j = 0; j < expectedRow.size(); ++j) {
//                         if (expectedRow[j] != actualRow[j]) {
//                             // std::cout << "Index " << j
//                             //           << ": Expected " << expectedRow[j]
//                             //           << ", Got " << actualRow[j] << std::endl;
//                         }
//                     }
//                     std::cout << "Row " << i << " does not match between single-threaded and multi-threaded results." << std::endl;
//                     std::cout << "Returning false" << std::endl;
//                 }
//
//                 return false;
//             }
//         }
//
//         if (verbose) {
//             std::cout << "All rows match between single-threaded and multi-threaded results." << std::endl;
//         }
//
//         return true;
//     }
//
//
//     void** concat_results(void*** partial_results, void** inputData, int thread_count, long long data_size) override {
//         auto* finalMatrix = new std::vector<std::vector<int>>(data_size);
//
//         for (int i = 0; i < thread_count; ++i) {
//             long long* range = calculate_area_of_responsibility(i, thread_count, data_size);
//             auto* partialResult = reinterpret_cast<std::vector<std::vector<int>>*>(partial_results[i]);
//
//             if (partialResult == nullptr) {
//                 throw std::runtime_error("Partial result is null for thread " + std::to_string(i));
//             }
//
//             for (long long row = range[0]; row < range[1]; ++row) {
//                 if (row - range[0] >= partialResult->size()) {
//                     throw std::out_of_range("Invalid row access in partial results.");
//                 }
//                 (*finalMatrix)[row] = (*partialResult)[row - range[0]];
//             }
//
//             delete partialResult;
//             delete[] range;
//         }
//
//         return reinterpret_cast<void**>(finalMatrix);
//     }
//
//
//     virtual std::vector<int> processRow(const std::vector<int>& row, const std::vector<std::vector<int>>& matrix) = 0;
// };
//
// class MatrixMultiplication : public MatrixOperationAlgorithm {
// public:
//     MatrixMultiplication(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
//         : MatrixOperationAlgorithm(threadCount, dataSize, verbose, reiterative) {}
//
// protected:
//     std::vector<int> processRow(const std::vector<int>& row, const std::vector<std::vector<int>>& matrix) override {
//         std::vector<std::vector<int>> localMatrixCopy = matrix; // Thread-safe local copy
//         std::vector<int> result(localMatrixCopy.size(), 0);
//         for (size_t col = 0; col < localMatrixCopy.size(); ++col) {
//             for (size_t k = 0; k < localMatrixCopy.size(); ++k) {
//                 result[col] += row[k] * localMatrixCopy[k][col];
//             }
//         }
//         return result;
//     }
// };
//
// class MatrixAddition : public MatrixOperationAlgorithm {
// public:
//     MatrixAddition(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
//         : MatrixOperationAlgorithm(threadCount, dataSize, verbose, reiterative) {}
//
// protected:
//     std::vector<int> processRow(const std::vector<int>& row, const std::vector<std::vector<int>>& matrix) override {
//         std::vector<std::vector<int>> localMatrixCopy = matrix; // Thread-safe local copy
//         std::vector<int> result(row.size());
//         for (size_t col = 0; col < row.size(); ++col) {
//             result[col] = row[col] + localMatrixCopy[col][col];
//         }
//         return result;
//     }
// };
//
//
// class MatrixTransposition : public MatrixOperationAlgorithm {
// public:
//     MatrixTransposition(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
//         : MatrixOperationAlgorithm(threadCount, dataSize, verbose, reiterative) {}
//
// protected:
//     std::vector<int> processRow(const std::vector<int>& row, const std::vector<std::vector<int>>& matrix) override {
//         std::vector<std::vector<int>> localMatrixCopy = matrix; // Thread-safe local copy
//         std::vector<int> result(localMatrixCopy.size());
//         size_t rowIndex = &row - &matrix[0];
//         for (size_t col = 0; col < localMatrixCopy.size(); ++col) {
//             result[col] = localMatrixCopy[col][rowIndex];
//         }
//         return result;
//     }
// };
//

