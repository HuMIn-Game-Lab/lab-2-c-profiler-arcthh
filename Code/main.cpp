#include "profiler.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>

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

void runTest() {
    test1();
    //test2();
    //test3();
}

int main() {
    profiler = Profiler::GetInstance();
    runTest();
    profiler->calculateStats();  // Aggregate the statistics
    profiler->printStatsToCSV("profile_stats.csv");
    profiler->printStatsToJSON("profile_stats.json");

    delete profiler;
    profiler = nullptr;
    return 0;
}
