#include "profiler.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::milliseconds

Profiler* profiler = nullptr;

// Function to start Python HTTP server
void startServer() {
    // First kill any existing Python servers
    #ifdef _WIN32
    system("taskkill /F /IM python.exe > nul 2>&1");
    system("start /B python -m http.server 8080");
    #else
    system("pkill -f 'python3 -m http.server' > /dev/null 2>&1");
    system("python3 -m http.server 8080 &");
    #endif
    
    // Give the server time to start
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

// Function to open browser for both visualizers
void openBrowsers() {
    #ifdef _WIN32
    system("start http://localhost:8080/Code/index.html");
    #elif __APPLE__
    system("open http://localhost:8080/Code/index.html");
    #else
    system("xdg-open http://localhost:8080/Code/index.html");
    #endif
}

void cleanupServer() {
    #ifdef _WIN32
    system("taskkill /F /IM python.exe > nul 2>&1");
    #else
    system("pkill -f 'python3 -m http.server' > /dev/null 2>&1");
    #endif
}


// Generate a large random array
std::vector<int> generateRandomArray(int size) {
    std::vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 10000;  // Random numbers between 0 and 9999
    }
    return arr;
}

void insertionSort(std::vector<int>& arr) {
    PROFILER_ENTER("Baseline Insertion Sort");
    int n = arr.size();
    
    PROFILER_ENTER("Array Traversal");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Single Element Insertion");
        int key = arr[i];
        int j = i - 1;
        
        PROFILER_ENTER("Element Shifting");
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
        PROFILER_EXIT("Element Shifting");
        
        PROFILER_EXIT("Single Element Insertion");
    }
    PROFILER_EXIT("Array Traversal");
    
    PROFILER_EXIT("Baseline Insertion Sort");
}

void insertionSortOptimized1(std::vector<int>& arr) {
    PROFILER_ENTER("Optimized Insertion Sort - Shifting");
    int n = arr.size();
    
    PROFILER_ENTER("Array Processing");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Element Processing");
        int key = arr[i];
        int j = i - 1;
        
        PROFILER_ENTER("Shift Operation");
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        PROFILER_EXIT("Shift Operation");
        
        PROFILER_ENTER("Key Placement");
        arr[j + 1] = key;
        PROFILER_EXIT("Key Placement");
        
        PROFILER_EXIT("Element Processing");
    }
    PROFILER_EXIT("Array Processing");
    
    PROFILER_EXIT("Optimized Insertion Sort - Shifting");
}

int binarySearch(const std::vector<int>& arr, int item, int low, int high) {
    PROFILER_ENTER("Binary Search Operation");
    int result = low;
    
    PROFILER_ENTER("Search Loop");
    while (low <= high) {
        int mid = (low + high) / 2;
        if (item == arr[mid]) {
            result = mid + 1;
            break;
        }
        else if (item > arr[mid]) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }
    PROFILER_EXIT("Search Loop");
    
    if (result == low) {
        result = low;
    }
    
    PROFILER_EXIT("Binary Search Operation");
    return result;
}

void insertionSortOptimized2(std::vector<int>& arr) {
    PROFILER_ENTER("Optimized Insertion Sort - Binary Search");
    int n = arr.size();
    
    PROFILER_ENTER("Main Sort Loop");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Single Pass");
        int key = arr[i];
        int j = i - 1;
        
        PROFILER_ENTER("Position Search");
        int loc = binarySearch(arr, key, 0, j);
        PROFILER_EXIT("Position Search");
        
        PROFILER_ENTER("Array Shifting");
        while (j >= loc) {
            arr[j + 1] = arr[j];
            j--;
        }
        PROFILER_EXIT("Array Shifting");
        
        arr[loc] = key;
        PROFILER_EXIT("Single Pass");
    }
    PROFILER_EXIT("Main Sort Loop");
    
    PROFILER_EXIT("Optimized Insertion Sort - Binary Search");
}

void insertionSortOptimized3(std::vector<int>& arr) {
    PROFILER_ENTER("Optimized Insertion Sort - Early Exit");
    int n = arr.size();
    bool sorted = true;
    
    PROFILER_ENTER("Sorting Process");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Element Insertion");
        int key = arr[i];
        int j = i - 1;
        
        sorted = true;
        PROFILER_ENTER("Comparison and Shift");
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
            sorted = false;
        }
        PROFILER_EXIT("Comparison and Shift");
        
        arr[j + 1] = key;
        PROFILER_EXIT("Element Insertion");
        
        if (sorted) {
            PROFILER_ENTER("Early Exit Check");
            PROFILER_EXIT("Early Exit Check");
            break;
        }
    }
    PROFILER_EXIT("Sorting Process");
    
    PROFILER_EXIT("Optimized Insertion Sort - Early Exit");
}

void runTest(vector<int> arrCopy1, vector <int> arrCopy2, vector <int> arrCopy3, vector <int> arrCopy4 ) {
    // Profile Baseline Insertion Sort
    insertionSort(arrCopy1);
    
    // Profile Optimized Insertion Sort - Shifting
    insertionSortOptimized1(arrCopy2);

    // Profile Optimized Insertion Sort - Binary Search
    insertionSortOptimized2(arrCopy3);

    // Profile Optimized Insertion Sort - Early Exit
    insertionSortOptimized3(arrCopy4);

}


int main() {
    cleanupServer();
    // Seed for random number generation
    srand(time(0));

    // Generate a large random array of size 10000
    std::vector<int> arr = generateRandomArray(10000);

    // Make copies of the array for each sort to compare
    std::vector<int> arrCopy1 = arr;
    std::vector<int> arrCopy2 = arr;
    std::vector<int> arrCopy3 = arr;
    std::vector<int> arrCopy4 = arr;

    profiler = Profiler::GetInstance();

    runTest(arrCopy1, arrCopy2, arrCopy3, arrCopy4);
    profiler->calculateStats();  // Aggregate the statistics
    //profiler->printStats();
    profiler->printStatsToCSV("./data/profile_stats.csv");
    profiler->printStatsToJSON("./data/profile_stats.json");

    // Open the visualizer in the default browser - index.html
    cout << "Starting local server..." << endl;
    startServer();
    cout << "Opening visualizers in browser..." << endl;
    openBrowsers();
    cout << "Visualizers are running at:" << endl;
    cout << "1. http://localhost:8080/Code/index.html" << endl;
    cout << "Press Enter to exit and stop the server..." << endl;
    
    cin.get();
    
    // Cleanup
    cleanupServer();

    delete profiler;
    profiler = nullptr;
    return 0;
}
