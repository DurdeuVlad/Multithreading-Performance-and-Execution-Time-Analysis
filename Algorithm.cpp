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
    double threadCount, duration, dataSize, start, end, correct;

    Measurement(double threadCount, double duration, double dataSize, double start, double end, bool correct)
        : threadCount(threadCount), duration(duration), dataSize(dataSize), start(start), end(end), correct(correct) {}

    [[nodiscard]] std::string toString() const {
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

public:
    Algorithm(int threadCount, long long dataSize, bool verbose)
        : threadCount(threadCount), dataSize(dataSize), verbose(verbose) {}

    virtual ~Algorithm() = default;

    void** executeTask(long long* areaOfResponsibility, void** data) {
        void** localResult = execute(areaOfResponsibility, data);
        delete[] areaOfResponsibility;
        return localResult;
    }

    Measurement executeAndMeasure(int threads, long long dataSize) {
        // Generate data and prepare threads
        void** data = generateData(dataSize);
        std::vector<std::thread> threadPool;
        auto start = std::chrono::high_resolution_clock::now();

        void*** result = new void **[threads];
        for (int i = 0; i < threads; ++i) {
            long long* areaOfResponsibility = calculate_area_of_responsibility(i, threads, dataSize);
            threadPool.emplace_back([&result, areaOfResponsibility, data, this]() {
                result[0] = executeTask(areaOfResponsibility, data);
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

        delete[] data;

        return Measurement(
            static_cast<double>(threads),
            duration.count(),
            static_cast<double>(dataSize),
            start.time_since_epoch().count(),
            end.time_since_epoch().count(),
            results_are_correct
        );
    }
    virtual std::string getType() const = 0;

protected:
    virtual void** execute(long long* area_of_responsibility, void** inputData) = 0;
    virtual void** generateData(long dataSize) = 0;
    virtual long long* calculate_area_of_responsibility(int currentThread, int maxThreads, long long dataSize) = 0;
    virtual bool test_result(void** input_data, void** result, long long dataSize) = 0;
    virtual void ** concat_results(void*** partial_results, void** inputData, int thread_count, long long data_size) = 0;
};

// ===================== SortingAlgorithm =====================
class SortingAlgorithm : public Algorithm {
public:
    SortingAlgorithm(int threadCount, long long dataSize)
        : Algorithm(threadCount, dataSize, verbose) {}

    [[nodiscard]] std::string getType() const override {
        return "SortingAlgorithm";
    }

protected:
    void** concat_results(void*** partial_results, void** inputData, int thread_count, long long data_size) override {
        // Allocate merged data
        auto* merged_data = new std::vector<int>();
        merged_data->reserve(data_size); // Reserve space for the merged data
        auto* inputDataProcessed = reinterpret_cast<std::vector<int>*>(inputData);

        // create vectors from indexes from partial results
        std::vector<long long> start (thread_count);
        std::vector<long long> end (thread_count);
        std::vector<long long> indexes (thread_count);
        for (int i = 0; i < thread_count; i++) {
            long long* area_of_responsibility = calculate_area_of_responsibility(i, thread_count, data_size);
            start[i] = area_of_responsibility[0];
            end[i] = area_of_responsibility[1];
            indexes[i] = start[i];
        }

        while (true) {
            int min = INT_MAX;
            int min_index = -1;
            for (int i = 0; i < thread_count; i++) {
                if (indexes[i] < end[i]) {
                    if (inputDataProcessed->at(indexes[i]) < min) {
                        min = inputDataProcessed->at(indexes[i]);
                        min_index = i;
                    }
                }
            }
            if (min_index == -1) {
                break;
            }
            merged_data->push_back(min);
            indexes[min_index]++;
        }

        std::cout << "Merged data: " << std::endl;
        // for (int j : *merged_data) {
        //     std::cout << j << " ";
        // }
        std::cout << "Merged data end" << std::endl;
        // Return merged data
        return reinterpret_cast<void**>(merged_data);
    }


    bool test_result(void **input_data, void **result, long long dataSize) override {
        auto input_data_processed = static_cast<int *>(result[0]);
        for (long long i=2;i<dataSize-1;i++) {
            if(input_data_processed[i-1]>input_data_processed[i]) {
                std::cout << "Sorting failed" << std::endl;
                std::cout << "Index: " << i << " Value: " << input_data_processed[i] << std::endl;
                std::cout << "Index: " << i-1 << " Value: " << input_data_processed[i-1] << std::endl;
                for (int j=0;j<dataSize;j++) {
                    std::cout << input_data_processed[j] << " ";
                }
                return false;
            }
        }
        return true;
    }

    void** execute(long long* area_of_responsibility, void** inputData) override {
        auto* data = reinterpret_cast<std::vector<int>*>(inputData);
        int start = static_cast<int>(area_of_responsibility[0]);
        int end = static_cast<int>(area_of_responsibility[1]);
        sortSegment(*data, start, end);
        return nullptr;
    }

    void** generateData(long dataSize) override {
        auto* data = new std::vector<int>(dataSize);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1000);
        std::generate(data->begin(), data->end(), [&]() { return dis(gen); });
        return reinterpret_cast<void**>(data);
    }

    long long* calculate_area_of_responsibility(int currentThread, int maxThreads, long long dataSize) override{
        auto* range = new long long[2];
        long long segmentSize = dataSize / maxThreads;
        range[0] = currentThread * segmentSize;
        range[1] = (currentThread == maxThreads - 1) ? dataSize-1 : (currentThread + 1) * segmentSize;
        return range;
    }



    virtual void sortSegment(std::vector<int>& data, int start, int end) = 0;
};

// ===================== BubbleSort =====================
class BubbleSort : public SortingAlgorithm {
public:
    BubbleSort(int threadCount, long long dataSize)
        : SortingAlgorithm(threadCount, dataSize) {}

    [[nodiscard]] std::string getType() const override {
        return "BubbleSort";
    }

protected:
    void sortSegment(std::vector<int>& data, int start, int end) override {
        for (int i = start; i < end - 1; ++i) {
            for (int j = start; j < end - 1 - (i - start); ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
};

class QuickSort : public SortingAlgorithm {
public:
    QuickSort(int threadCount, long long dataSize)
        : SortingAlgorithm(threadCount, dataSize) {}

    [[nodiscard]] std::string getType() const override {
        return "QuickSort";
    }

protected:
    void sortSegment(std::vector<int>& data, int start, int end) override {
        std::sort(data.begin() + start, data.begin() + end);
    }
};

class MergeSort : public SortingAlgorithm {
public:
    MergeSort(int threadCount, long long dataSize)
        : SortingAlgorithm(threadCount, dataSize) {}

    [[nodiscard]] std::string getType() const override {
        return "MergeSort";
    }

protected:
    void sortSegment(std::vector<int>& data, int start, int end) override {
        if (end - start <= 1) return;
        int mid = start + (end - start) / 2;
        sortSegment(data, start, mid);
        sortSegment(data, mid, end);
        std::inplace_merge(data.begin() + start, data.begin() + mid, data.begin() + end);
    }
};

class InsertionSort : public SortingAlgorithm {
public:
    InsertionSort(int threadCount, long long dataSize)
        : SortingAlgorithm(threadCount, dataSize) {}

    [[nodiscard]] std::string getType() const override {
        return "InsertionSort";
    }

protected:
    void sortSegment(std::vector<int>& data, int start, int end) override {
        for (int i = start + 1; i < end; ++i) {
            int key = data[i];
            int j = i - 1;
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
    SelectionSort(int threadCount, long long dataSize)
        : SortingAlgorithm(threadCount, dataSize) {}

    [[nodiscard]] std::string getType() const override {
        return "SelectionSort";
    }

protected:
    void sortSegment(std::vector<int>& data, int start, int end) override {
        for (int i = start; i < end - 1; ++i) {
            int minIndex = i;
            for (int j = i + 1; j < end; ++j) {
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
    HeapSort(int threadCount, long long dataSize)
        : SortingAlgorithm(threadCount, dataSize) {}

    [[nodiscard]] std::string getType() const override {
        return "HeapSort";
    }

protected:
    void sortSegment(std::vector<int>& data, int start, int end) override {
        for (int i = end / 2 - 1; i >= start; --i) {
            heapify(data, end, i);
        }
        for (int i = end - 1; i > start; --i) {
            std::swap(data[start], data[i]);
            heapify(data, i, start);
        }
    }
    void heapify(std::vector<int>& data, int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
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


