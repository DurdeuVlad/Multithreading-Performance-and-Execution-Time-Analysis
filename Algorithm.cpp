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
#include <stdexcept>
#include "json.hpp"

// ===================== Measurement =====================
class Measurement {
public:
    double threadCount, duration, dataSize;
    long long start, end;
    bool correct;
    bool* iterative;

    Measurement(double threadCount, double duration, double dataSize, long long start, long long end, bool correct, bool* iterative = nullptr)
        : threadCount(threadCount), duration(duration), dataSize(dataSize), start(start), end(end), correct(correct), iterative(iterative) {}

    Measurement() = default;

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

    [[nodiscard]] nlohmann::json toJson() const {
        nlohmann::json j;
        j["duration"] = duration;
        j["start"] = start;
        j["end"] = end;
        j["correct"] = correct;
        if (iterative != nullptr) {
            j["iterative"] = *iterative;
        } else {
            j["iterative"] = false;
        }
        return j;
    }

    bool operator==(const Measurement & measurement) const {
        return threadCount == measurement.threadCount &&
               duration == measurement.duration &&
               dataSize == measurement.dataSize &&
               start == measurement.start &&
               end == measurement.end &&
               correct == measurement.correct;
    };
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

    std::vector<int*> executeTask(const std::vector<long long>& areaOfResponsibility, const std::vector<int*>& data, std::atomic<bool>& stopFlag) {
        return execute(areaOfResponsibility, data, stopFlag);
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
        std::atomic<bool> stopFlag = false;

        auto start = std::chrono::high_resolution_clock::now();

        try {
            for (int i = 0; i < threads; ++i) {
                auto areaOfResponsibility = calculate_area_of_responsibility(i, threads, dataSize);

                if (verbose) {
                    std::cout << "Thread " << i << " responsible for rows [" << areaOfResponsibility[0] << ", "
                              << areaOfResponsibility[1] << "]." << std::endl;
                }

                threadPool.emplace_back([&, i, areaOfResponsibility]() {
                    try {
                        if (!stopFlag) {
                            result[i] = executeTask(areaOfResponsibility, data, stopFlag);
                        }
                        if (verbose && stopFlag) {
                            std::lock_guard<std::mutex> lock(outputMutex);
                            std::cout << "Thread " << i << " stopped early." << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Thread " << i << " encountered an exception: " << e.what() << std::endl;
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
    std::mutex outputMutex; // For synchronizing output

    virtual std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData, std::atomic<bool>& stopFlag) = 0;
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

    std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData, std::atomic<bool>&  stopFlag) override {
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

class MatrixOperationAlgorithm : public Algorithm {
public:
    MatrixOperationAlgorithm(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : Algorithm(threadCount, dataSize, verbose, reiterative) {}

    [[nodiscard]] std::string getType() const override {
        return "MatrixOperationAlgorithm";
    }

protected:
    void cleanupData(std::vector<int*>& data, std::vector<std::vector<int*>>& result) const override {
        for (auto& row : data) {
            delete[] row;
        }
        data.clear();
        for (auto& partial_result : result) {
            for (auto& row : partial_result) {
                delete[] row;
            }
        }
        result.clear();
    }

    std::vector<int*> generateData(long long dataSize) override {
        std::vector<int*> matrix(dataSize);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 100);
        for (long long i = 0; i < dataSize; ++i) {
            matrix[i] = new int[dataSize];
            for (long long j = 0; j < dataSize; ++j) {
                matrix[i][j] = dis(gen);
            }
        }
        return matrix;
    }

    std::vector<long long> calculate_area_of_responsibility(int currentThread, int maxThreads, long long dataSize) override {
        long long segmentSize = dataSize / maxThreads;
        return {currentThread * segmentSize, (currentThread == maxThreads - 1) ? dataSize : (currentThread + 1) * segmentSize};
    }

    std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData, std::atomic<bool>&  stopFlag) override {
        std::vector<int*> partialResult;
        for (long long i = area_of_responsibility[0]; i < area_of_responsibility[1]; ++i) {
            partialResult.push_back(processRow(inputData[i], inputData));
        }
        return partialResult;
    }

    bool test_result(const std::vector<int*>& input_data, const std::vector<int*>& result, long long dataSize) override {
        for (long long i = 0; i < dataSize; ++i) {
            auto expected = processRow(input_data[i], input_data);
            for (long long j = 0; j < dataSize; ++j) {
                if (expected[j] != result[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    std::vector<int*> concat_results(const std::vector<std::vector<int*>>& partial_results, const std::vector<int*>& inputData, int thread_count, long long data_size) override {
        std::vector<int*> finalMatrix(data_size);
        for (int i = 0; i < thread_count; ++i) {
            auto area = calculate_area_of_responsibility(i, thread_count, data_size);
            for (long long j = area[0]; j < area[1]; ++j) {
                finalMatrix[j] = partial_results[i][j - area[0]];
            }
        }
        return finalMatrix;
    }

    virtual int* processRow(const int* row, const std::vector<int*>& matrix) = 0;
};

class MatrixMultiplication : public MatrixOperationAlgorithm {
public:
    MatrixMultiplication(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : MatrixOperationAlgorithm(threadCount, dataSize, verbose, reiterative) {}

protected:
    int* processRow(const int* row, const std::vector<int*>& matrix) override {
        long long size = matrix.size();
        int* result = new int[size];
        std::fill(result, result + size, 0);
        for (long long col = 0; col < size; ++col) {
            for (long long k = 0; k < size; ++k) {
                result[col] += row[k] * matrix[k][col];
            }
        }
        return result;
    }
};

class MatrixAddition : public MatrixOperationAlgorithm {
public:
    MatrixAddition(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : MatrixOperationAlgorithm(threadCount, dataSize, verbose, reiterative) {}

protected:
    int* processRow(const int* row, const std::vector<int*>& matrix) override {
        long long size = matrix.size();
        int* result = new int[size];
        for (long long col = 0; col < size; ++col) {
            result[col] = row[col] + matrix[col][col];
        }
        return result;
    }
};

class MatrixTransposition : public MatrixOperationAlgorithm {
public:
    MatrixTransposition(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : MatrixOperationAlgorithm(threadCount, dataSize, verbose, reiterative) {}

protected:
    int* processRow(const int* row, const std::vector<int*>& matrix) override {
        long long size = matrix.size();
        int* result = new int[size];
        long long rowIndex = std::distance(matrix.begin(), std::find(matrix.begin(), matrix.end(), row));
        for (long long col = 0; col < size; ++col) {
            result[col] = matrix[col][rowIndex];
        }
        return result;
    }
};

class SearchAlgorithms : public Algorithm {
protected:
    int targetNumber;
    std::atomic<bool> found;
    std::atomic<long long> foundIndex;

public:
    SearchAlgorithms(int threadCount, long long dataSize, bool verbose = false, bool* reiterative = nullptr)
        : Algorithm(threadCount, dataSize, verbose, reiterative), found(false), foundIndex(-1) {}

    [[nodiscard]] virtual std::string getType() const override = 0;

protected:
    std::vector<int*> generateData(long long dataSize) override {
        auto* data = new int[dataSize];
        for (long long i = 0; i < dataSize; ++i) {
            data[i] = i + 1;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<long long> indexDis(0, dataSize - 1);
        long long targetIndex = indexDis(gen);
        targetNumber = data[targetIndex];
        if (verbose) {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "Target number: " << targetNumber << " placed at index: " << targetIndex << std::endl;
        }
        return {data};
    }

    std::vector<long long> calculate_area_of_responsibility(int currentThread, int maxThreads, long long dataSize) override {
        long long segmentSize = dataSize / maxThreads;
        return {currentThread * segmentSize, (currentThread == maxThreads - 1) ? dataSize : (currentThread + 1) * segmentSize};
    }

    std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData, std::atomic<bool>& stopFlag) override {
        for (long long i = area_of_responsibility[0]; i < area_of_responsibility[1] && !stopFlag; ++i) {
            if (inputData[0][i] == targetNumber) {
                found = true;
                foundIndex = i;
                stopFlag = true;
                if (verbose) {
                    std::lock_guard<std::mutex> lock(outputMutex);
                    std::cout << "Thread found the number " << targetNumber << " at index " << i << std::endl;
                }
                break;
            }
        }
        return {};
    }

    bool test_result(const std::vector<int*>& input_data, const std::vector<int*>&, long long dataSize) override {
        if (foundIndex < 0 || foundIndex >= dataSize || input_data[0][foundIndex] != targetNumber) {
            if (verbose) {
                std::cerr << "Test failed. Target number " << targetNumber << " was not correctly found." << std::endl;
            }
            return false;
        }
        return true;
    }

    std::vector<int*> concat_results(const std::vector<std::vector<int*>>& partial_results, const std::vector<int*>& inputData, int, long long) override {
        return inputData;
    }

    void cleanupData(std::vector<int*>& data, std::vector<std::vector<int*>>& result) const override {
        if (!data.empty() && data[0] != nullptr) {
            delete[] data[0];
            data[0] = nullptr;
        }
        data.clear();
        for (auto& res : result) {
            if (!res.empty() && res[0] != nullptr) {
                delete[] res[0];
                res[0] = nullptr;
            }
        }
        result.clear();
    }
};


class LinearSearch : public SearchAlgorithms {
public:
    LinearSearch(int threadCount, long long dataSize, bool verbose = false)
        : SearchAlgorithms(threadCount, dataSize, verbose) {}

    [[nodiscard]] std::string getType() const override {
        return "LinearSearch";
    }

protected:
    std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData, std::atomic<bool>& stopFlag) override {
        for (long long i = area_of_responsibility[0]; i < area_of_responsibility[1] && !stopFlag; ++i) {
            if (inputData[0][i] == targetNumber) {
                found = true;
                foundIndex = i;
                stopFlag = true;
                if (verbose) {
                    std::lock_guard<std::mutex> lock(outputMutex);
                    std::cout << "Thread found the number " << targetNumber << " at index " << i << std::endl;
                }
                break;
            }
        }
        return {};
    }
};


class BinarySearch : public SearchAlgorithms {
public:
    BinarySearch(int threadCount, long long dataSize, bool verbose = false)
        : SearchAlgorithms(threadCount, dataSize, verbose) {}

    [[nodiscard]] std::string getType() const override {
        return "BinarySearch";
    }

protected:
    std::vector<int*> execute(const std::vector<long long>& area_of_responsibility, const std::vector<int*>& inputData, std::atomic<bool>& stopFlag) override {
        long long start = area_of_responsibility[0];
        long long end = area_of_responsibility[1];
        while (start < end && !stopFlag) {
            long long mid = start + (end - start) / 2;
            if (inputData[0][mid] == targetNumber) {
                found = true;
                foundIndex = mid;
                stopFlag = true;
                if (verbose) {
                    std::lock_guard<std::mutex> lock(outputMutex);
                    std::cout << "Thread found the number " << targetNumber << " at index " << mid << std::endl;
                }
                break;
            } else if (inputData[0][mid] < targetNumber) {
                start = mid + 1;
            } else {
                end = mid;
            }
        }
        return {};
    }

    std::vector<int*> generateData(long long dataSize) override {
        auto data = SearchAlgorithms::generateData(dataSize);
        std::sort(data[0], data[0] + dataSize); // Ensure data is sorted for binary search
        return data;
    }
};

