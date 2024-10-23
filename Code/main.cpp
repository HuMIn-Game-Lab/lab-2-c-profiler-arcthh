#include "profiler.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::milliseconds


constexpr float DEGREES_TO_RADIANS = (3.14159f / 360.f);

Profiler* profiler = nullptr;

void test1(){
    PROFILER_ENTER("Trig Speed Test");
    constexpr int TRIG_TEST_NUM_ENTRIES =100'000;
    float randomYawDegreeTable[TRIG_TEST_NUM_ENTRIES] = {};  

    PROFILER_ENTER("Random Angle Generation");
    for (int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++) {
        randomYawDegreeTable[i] = 360.f * float(rand()) / float(RAND_MAX);
    }
    //profiler->ExitSection("Random Angle Generation");
    //profiler->ExitSection("Random Angle Generation", __LINE__, __FILE__, __FUNCTION__);
    PROFILER_EXIT("Random Angle Generation");
    float biggestSoFar = 0.f;

    PROFILER_ENTER("Total Cos and Sin Compute");
    for (int i=0; i < TRIG_TEST_NUM_ENTRIES; i++) {
        PROFILER_ENTER("Cos and Sin Compute");
        float yawDegrees = randomYawDegreeTable[i];
        float cosDegrees = cosf(yawDegrees* DEGREES_TO_RADIANS);
        float sinDegrees = sinf(yawDegrees* DEGREES_TO_RADIANS);
        PROFILER_EXIT("Cos and Sin Compute");
        if (cosDegrees + sinDegrees > biggestSoFar) {
            biggestSoFar = cosDegrees + sinDegrees;
        }
    }
    PROFILER_EXIT("Total Cos and Sin Compute");
    PROFILER_EXIT("Trig Speed Test");
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl;

}
void test2(){

    PROFILER_ENTER("Trig Speed Test");
    constexpr int TRIG_TEST_NUM_ENTRIES =100'000;
    float randomYawDegreeTable[TRIG_TEST_NUM_ENTRIES] = {};  

    PROFILER_ENTER("Random Angle Generation");
    for (int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++) {
        randomYawDegreeTable[i] = 360.f * float(rand()) / float(RAND_MAX);
    }
    //profiler->ExitSection("Random Angle Generation");
    //profiler->ExitSection("Random Angle Generation", __LINE__, __FILE__, __FUNCTION__);
    PROFILER_EXIT("Random Angle Generation");
    float biggestSoFar = 0.f;

    PROFILER_ENTER("Total Cos and Sin Compute");
    for (int i=0; i < TRIG_TEST_NUM_ENTRIES; i++) {
        PROFILER_ENTER("Cos and Sin Compute");
        float yawDegrees = randomYawDegreeTable[i];
        float cosDegrees = cosf(yawDegrees* DEGREES_TO_RADIANS);
        float sinDegrees = sinf(yawDegrees* DEGREES_TO_RADIANS);
        PROFILER_EXIT("Cos and Sin Compute");
        if (cosDegrees + sinDegrees > biggestSoFar) {
            biggestSoFar = cosDegrees + sinDegrees;
        }
    }
    PROFILER_EXIT("Total Cos and Sin Compute");
    PROFILER_EXIT("Trig Speed Test");
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl;

};

void test3(){
    ProfilerScopeObject myObject("Trig Speed Test");
    //PROFILER_ENTER("Trig Speed Test");
    constexpr int TRIG_TEST_NUM_ENTRIES =100'000;
    float randomYawDegreeTable[TRIG_TEST_NUM_ENTRIES] = {};  

    //PROFILER_ENTER("Random Angle Generation");
    for (int i = 0; i < TRIG_TEST_NUM_ENTRIES; i++) {
        randomYawDegreeTable[i] = 360.f * float(rand()) / float(RAND_MAX);
    }
    //profiler->ExitSection("Random Angle Generation");
    //profiler->ExitSection("Random Angle Generation", __LINE__, __FILE__, __FUNCTION__);
    //PROFILER_EXIT("Random Angle Generation");
    float biggestSoFar = 0.f;

   // PROFILER_ENTER("Total Cos and Sin Compute");
    for (int i=0; i < TRIG_TEST_NUM_ENTRIES; i++) {
     //   PROFILER_ENTER("Cos and Sin Compute");
        float yawDegrees = randomYawDegreeTable[i];
        float cosDegrees = cosf(yawDegrees* DEGREES_TO_RADIANS);
        float sinDegrees = sinf(yawDegrees* DEGREES_TO_RADIANS);
    //    PROFILER_EXIT("Cos and Sin Compute");
        if (cosDegrees + sinDegrees > biggestSoFar) {
            biggestSoFar = cosDegrees + sinDegrees;
        }
    }
    // PROFILER_EXIT("Total Cos and Sin Compute");
    // PROFILER_EXIT("Trig Speed Test");
    std::cout << "Biggest cos+sin = " << biggestSoFar << std::endl;
}
void testInterleaving() {
    PROFILER_ENTER("Main Task");
    constexpr int NUM_ITERATIONS = 50;  // You can change the iteration count as needed
    float result = 0.0f;

    // Start Subtask 1
    PROFILER_ENTER("Subtask 1");
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        result += static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    // Instead of ending Subtask 1 here, we interleave with another task
    
    // Start Subtask 2 before Subtask 1 finishes
    PROFILER_ENTER("Subtask 2");
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        result += static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f;
    }
    // Now finish Subtask 1
    PROFILER_EXIT("Subtask 1");

    // Continue Subtask 2 after Subtask 1 has finished
    for (int i = 0; i < NUM_ITERATIONS / 2; ++i) {
        result += static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.5f;
    }
    PROFILER_EXIT("Subtask 2");

    // End the Main Task
    PROFILER_EXIT("Main Task");

    // Print out a simple result from the calculation
    std::cout << "Interleaving result: " << result << std::endl;
}

void runTest() {
    test1();
    //test2();
    //test3();
}

void testHierarchicalProfiling() {
    // Enter nested sections A, B, C
    PROFILER_ENTER("Section A");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate work in A

    PROFILER_ENTER("Section B");
    std::this_thread::sleep_for(std::chrono::milliseconds(150));  // Simulate work in B

    PROFILER_ENTER("Section C");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));  // Simulate work in C

    // Leave sections in reverse order
    PROFILER_EXIT("Section C");  // Exiting C
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Simulate more work in B

    PROFILER_EXIT("Section B");  // Exiting B
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Simulate more work in A

    PROFILER_EXIT("Section A");  // Exiting A

    std::cout << "Hierarchical profiling test completed." << std::endl;
}

void testInterleavedProfiling() {
    // Enter interleaved sections A and B
    PROFILER_ENTER("Section A");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate work in A

    PROFILER_ENTER("Section B");
    std::this_thread::sleep_for(std::chrono::milliseconds(150));  // Simulate work in B

    // Interleave the exits: Leave A before B
    PROFILER_EXIT("Section A");  // Exiting A before B
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate more work in B

    PROFILER_EXIT("Section B");  // Exiting B after A

    std::cout << "Interleaved profiling test completed." << std::endl;
}

int main() {
    profiler = Profiler::GetInstance();
    //testInterleaving();
    testHierarchicalProfiling();
    testInterleavedProfiling();
    runTest();
    profiler->calculateStats();  // Aggregate the statistics
    profiler->printStatsToCSV("./data/profile_stats.csv");
    profiler->printStatsToJSON("./data/profile_stats.json");

    delete profiler;
    profiler = nullptr;
    return 0;
}
