#pragma once
#include <cfloat>  // For DBL_MAX (used for setting min/max time)
#include <vector>
#include <map>
#include <iostream>
#include <iomanip> // For formatting the output
#include <fstream>
#include <limits>


// Macros for entering and exiting profiling sections
#define PROFILER_ENTER(sectionName) Profiler::GetInstance()->EnterSection(sectionName);
#define PROFILER_EXIT(sectionName) Profiler::GetInstance()->ExitSection(sectionName, __LINE__, __FILE__, __FUNCTION__);

using namespace std;

// ProfilerScopeObject class: Used for automatically starting and stopping profiling when an object goes in and out of scope
class ProfilerScopeObject {
    public:
        ProfilerScopeObject(char const* sectionName);
        ~ProfilerScopeObject();
        char const* sectionName;
};

// TimeRecordStart class: Holds the start time of a section
class TimeRecordStart {
    public:
        TimeRecordStart(char const* sectionName, double secondsAtStart);
        ~TimeRecordStart();
        char const* sectionName;
        double secondsAtStart;
};

// TimeRecordStop class: Holds the stop time and metadata for a section
class TimeRecordStop {
    public:
        TimeRecordStop(char const* sectionName, double secondsAtStop);
        TimeRecordStop(char const* sectionName, double secondsAtStop, int lineNumber, const char* fileName, const char* functionName);
        ~TimeRecordStop();

        char const* sectionName;
        double elapsedTime;
        int lineNumber;
        const char* fileName;
        const char* functionName;
};
// ProfilerStats class: Holds aggregated statistics for a specific section
class ProfilerStats {
    public:
        ProfilerStats(char const* sectionName);
        ~ProfilerStats();

        char const* sectionName;
        int count;
        double totalTime;
        double minTime;
        double maxTime;
        double avgTime;

        const char* fileName;
        const char* functionName;
        int lineNumber;
};

// Profiler class: Singleton pattern to manage profiling across the entire application
class Profiler {
    public:
        ~Profiler();

        // Methods for starting and ending profiling sections
        void EnterSection(char const* sectionName);
        void ExitSection(char const* sectionName);
        void ExitSection(char const* sectionName, int lineNumber, const char* fileName, const char* functionName);

        // Method to calculate statistics for all sections
        void calculateStats();

        // Method to print profiling statistics 
        void printStats();
        void printStatsToCSV(const char* fileName);
        void printStatsToJSON(const char* fileName);

        // Singleton pattern to ensure only one instance of Profiler exists
        static Profiler* gProfiler;  // Static instance of the Profiler
        static Profiler* GetInstance(); //singleton pattern

    private:
        // Private constructor to enforce the singleton pattern
        Profiler();
        void ReportSectionTime(char const* sectionName, double elapsedTime);
        void ReportSectionTime(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName);

        // Map to store profiling statistics by section name
        std::map<char const*, ProfilerStats*> stats;
        std::vector<TimeRecordStop> elapsedTimes;
        std::map<const char*, TimeRecordStart> sectionMap;
};
