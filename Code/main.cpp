#include "profiler.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::milliseconds

Profiler* profiler = nullptr;

using namespace std;

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

void baselineInsertionSort(std::vector<int>& arr) {
    PROFILER_ENTER("Baseline Insertion Sort");
    int n = arr.size();
    
    PROFILER_ENTER("Insertion Sort1: Outer Loop");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Insertion Sort1: Key Selection");
        int key = arr[i];
        int j = i - 1;
        
        PROFILER_ENTER("Insertion Sort1: Element Shifting");
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
        PROFILER_EXIT("Insertion Sort1: Element Shifting");
        
        PROFILER_EXIT("Insertion Sort1: Key Selection");
    }
    PROFILER_EXIT("Insertion Sort1: Outer Loop");    
    PROFILER_EXIT("Baseline Insertion Sort");
}

void insertionSortShifting2(std::vector<int>& arr) {
    PROFILER_ENTER("Optimized Insertion Sort2 - Shifting");
    int n = arr.size();
    
    PROFILER_ENTER("Insertion Sort2: Outer Loop");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Insertion Sort2: Key Selection");
        int key = arr[i];
        int j = i - 1;
        
        PROFILER_ENTER("Insertion Sort2: Element Shifting");
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        PROFILER_EXIT("Insertion Sort2: Element Shifting");
        
        PROFILER_ENTER("Insertion Sort2: Key Placement");
        arr[j + 1] = key;
        PROFILER_EXIT("Insertion Sort2: Key Placement");
        
        PROFILER_EXIT("Insertion Sort2: Key Selection");
    }
    PROFILER_EXIT("Insertion Sort2: Outer Loop");
    
    PROFILER_EXIT("Optimized Insertion Sort2 - Shifting");
}

int binarySearch(const std::vector<int>& arr, int item, int low, int high) {
    //PROFILER_ENTER("Binary Search Operation");
    int result = low;
    
    //PROFILER_ENTER("Search Loop");
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
    //PROFILER_EXIT("Search Loop");
    
    if (result == low) {
        result = low;
    }
    
    //PROFILER_EXIT("Binary Search Operation");
    return result;
}

void insertionSortBinary3(std::vector<int>& arr) {
    PROFILER_ENTER("Optimized Insertion Sort3 - Binary Search");
    int n = arr.size();
    
    PROFILER_ENTER("Insertion Sort3: Outer Loop");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Insertion Sort3: Key Selection");
        int key = arr[i];
        int j = i - 1;
        
        PROFILER_ENTER("Insertion Sort3: Binary Search");
        int loc = binarySearch(arr, key, 0, j);
        PROFILER_EXIT("Insertion Sort3: Binary Search");
        
        PROFILER_ENTER("Insertion Sort3: Element Shifting");
        while (j >= loc) {
            arr[j + 1] = arr[j];
            j--;
        }
        PROFILER_EXIT("Insertion Sort3: Element Shifting");
        
        arr[loc] = key;
        PROFILER_EXIT("Insertion Sort3: Key Selection");
    }
    PROFILER_EXIT("Insertion Sort3: Outer Loop");
    
    PROFILER_EXIT("Optimized Insertion Sort3 - Binary Search");
}

void insertionSortEarlyExit4(std::vector<int>& arr) {
    PROFILER_ENTER("Optimized Insertion Sort4 - Early Exit");
    int n = arr.size();
    bool sorted = true;
    
    PROFILER_ENTER("Insertion Sort4: Outer Loop");
    for (int i = 1; i < n; i++) {
        PROFILER_ENTER("Insertion Sort4: Key Selection");
        int key = arr[i];
        int j = i - 1;
        
        sorted = true;
        PROFILER_ENTER("Insertion Sort4: Element Shifting");
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
            sorted = false;
        }
        PROFILER_EXIT("Insertion Sort4: Element Shifting");
        
        arr[j + 1] = key;
        PROFILER_EXIT("Insertion Sort4: Key Selection");
        
        if (sorted) {
            PROFILER_ENTER("Insertion Sort4: Early Exit Check");
            PROFILER_EXIT("Insertion Sort4: Early Exit Check");
            break;
        }
    }
    PROFILER_EXIT("Insertion Sort4: Outer Loop");
    
    PROFILER_EXIT("Optimized Insertion Sort4 - Early Exit");
}

void runTest(vector<int> arrCopy1, vector <int> arrCopy2, vector <int> arrCopy3, vector <int> arrCopy4 ) {
    // Profile Baseline Insertion Sort
    baselineInsertionSort(arrCopy1);
    
    // Profile Optimized Insertion Sort - Shifting
    insertionSortShifting2(arrCopy2);

    // Profile Optimized Insertion Sort - Binary Search
    insertionSortBinary3(arrCopy3);

    // Profile Optimized Insertion Sort - Early Exit
    insertionSortEarlyExit4(arrCopy4);

}


int main() {
    cleanupServer();
    // Seed for random number generation
    srand(time(0));

    // Generate a large random array of size 5000
    std::vector<int> arr = generateRandomArray(5000);

    // Make copies of the array for each sort to compare
    std::vector<int> arrCopy1 = arr;
    std::vector<int> arrCopy2 = arr;
    std::vector<int> arrCopy3 = arr;
    std::vector<int> arrCopy4 = arr;

    profiler = Profiler::GetInstance();

    runTest(arrCopy1, arrCopy2, arrCopy3, arrCopy4);
    profiler->calculateStats();  // Aggregate the statistics
    //profiler->printStats();
    // In main.cpp, update these lines to use the correct case
    profiler->printStatsToCSV("./Data/profile_stats.csv");
    profiler->printStatsToJSON("./Data/profile_stats.json");

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
