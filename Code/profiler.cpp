#include "profiler.hpp"
#include "time.hpp" //should be time.hpp
#include <iostream>
#include <iomanip> // For formatting the output
#include <fstream>
#include <limits>


//create a macro
#define PROFILER_ENTER(sectionName) Profiler::GetInstance()->EnterSection(sectionName);
#define PROFILER_EXIT(sectionName) Profiler::GetInstance()->ExitSection(sectionName, __LINE__, __FILE__, __FUNCTION__);
//This macro above will replace the ExitSection function call with the ExitSection function call with the additional parameters of __LINE__, __FILE__, __FUNCTION__

Profiler* Profiler::gProfiler = nullptr;

TimeRecordStart::TimeRecordStart(char const* sectionName, double secondsAtStart) : sectionName(sectionName), secondsAtStart(secondsAtStart) { };
TimeRecordStart::~TimeRecordStart() { };

TimeRecordStop::TimeRecordStop(char const* sectionName, double secondsAtStop, int lineNumber, const char* fileName, const char* functionName ) : sectionName(sectionName), elapsedTime(secondsAtStop), lineNumber(lineNumber), fileName(fileName), functionName(functionName) { };
TimeRecordStop::TimeRecordStop(char const* sectionName, double secondsAtStop ) : sectionName(sectionName), elapsedTime(secondsAtStop), lineNumber(0), fileName("null"), functionName("null") { };
TimeRecordStop::~TimeRecordStop() { };

//need to fix this
ProfilerScopeObject::ProfilerScopeObject(char const* sectionName) : sectionName(sectionName) {
    Profiler::GetInstance()->EnterSection(sectionName);
}

ProfilerScopeObject::~ProfilerScopeObject() {
    Profiler::GetInstance()->ExitSection(sectionName);
}

// ProfilerStats constructor definition
ProfilerStats::ProfilerStats(char const* sectionName)
    : sectionName(sectionName),
      count(0),
      totalTime(0.0),
      minTime(std::numeric_limits<double>::max()),  // Set to the maximum possible value
      maxTime(0.0),
      avgTime(0.0),
      fileName(nullptr),
      functionName(nullptr),
      lineNumber(0) {}

ProfilerStats::~ProfilerStats() {
    // Destructor can be empty if there is no dynamic memory allocation
}


Profiler::Profiler() {
    gProfiler = this;
    startTimes.reserve(100); //pre allocation
    elapsedTimes.reserve(1000000); //pre allocation

    //how coild i make this more dynamic? 
    //config files or macros to adjust how much were allocation and compile with a macro flag

}
//method checks to see if Gprofiler exists, if not, it will create a new Profiler and initialize
Profiler* Profiler::GetInstance() {
    if(gProfiler == nullptr) {
        gProfiler = new Profiler();
    }
    return gProfiler;
}

Profiler::~Profiler() {
    // for(auto& start : startTimes) {
    //     delete start;
    // }
    // for(auto& elapsed : elapsedTimes) {
    //     delete elapsed;
    // }
    // for(auto& stat : stats) {
    //     delete stat.second;
    // }
}   

void Profiler::EnterSection(char const* sectionName) {
    double secondsAtStart = GetCurrentTimeSeconds();
    startTimes.emplace_back(sectionName, secondsAtStart);
    //emplace_back is an optomized method that constructs the object in place,
    // push_back constructs the object and then moves it to the vector
    // TimeRecordStart* start = new TimeRecordStart(sectionName, secondsAtStart);
    // startTimes.push_back(start);

    // Push the section onto the stack
    //sectionStack.push(TimeRecordStart(sectionName, secondsAtStart));

}

void Profiler::ExitSection(char const* sectionName) {
    double secondsAtStop = GetCurrentTimeSeconds();
    TimeRecordStart const& currentSection = startTimes.back();

    #if defined(DEBUG_PROFILER)
    //verify the stack isn't empty
    
    //verify the currentSection matches the sectionName
    #endif

    double elapsedTime = secondsAtStop - currentSection.secondsAtStart; 
    //ReportSectionTime(sectionName, elapsedTime, __LINE__, __FILE__, __FUNCTION__);
    //commented out use of function ReportSectionTime to avoid overhead. Can grab information from macro that exists in the TimeRecordStop class
    elapsedTimes.emplace_back(sectionName, elapsedTime);
    startTimes.pop_back();
    
}

//exit section v2
void Profiler::ExitSection(char const* sectionName, int lineNumber, const char* fileName, const char* functionName) {
    double secondsAtStop = GetCurrentTimeSeconds();
    TimeRecordStart const& currentSection = startTimes.back();

    // #if defined(DEBUG_PROFILER)
    // //verify the stack isn't empty
    
    // //verify the currentSection matches the sectionName
    // #endif

    double elapsedTime = secondsAtStop - currentSection.secondsAtStart; 
    ReportSectionTime(sectionName, elapsedTime, lineNumber, fileName, functionName);
    //commented out use of function ReportSectionTime to avoid overhead. Can grab information from macro that exists in the TimeRecordStop class
   //commenting out for now elapsedTimes.emplace_back(sectionName, elapsedTime);
    startTimes.pop_back();
}

void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime) {
    elapsedTimes.emplace_back(sectionName, elapsedTime);
}

void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName) {
    elapsedTimes.emplace_back(sectionName, elapsedTime, lineNumber, fileName, functionName);
}

void Profiler::printStatsToCSV(const char* fileName) {
    std::ofstream file(fileName);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open file for CSV output." << std::endl;
        return;
    }

    // Write CSV headers
    file << "Section Name, Call Count, Total Time, Min Time, Max Time, Avg Time, File Name, Function Name, Line Number\n";

    // Iterate through the map of stats and output the details
    for (const auto& entry : stats) {
        ProfilerStats* stat = entry.second;
        file << stat->sectionName << ", " 
             << stat->count << ", " 
             << stat->totalTime << ", " 
             << stat->minTime << ", " 
             << stat->maxTime << ", " 
             << stat->avgTime << ", " 
             << stat->fileName << ", " 
             << stat->functionName << ", " 
             << stat->lineNumber << "\n";
    }

    file.close();
    std::cout << "Profiler stats written to " << fileName << " in CSV format.\n";
}

void Profiler::printStatsToJSON(const char* fileName) {
    std::ofstream file(fileName);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open file for JSON output." << std::endl;
        return;
    }

    // Start the JSON array
    file << "[\n";

    bool first = true;
    for (const auto& entry : stats) {
        ProfilerStats* stat = entry.second;
        
        if (!first) {
            file << ",\n";  // Add a comma between objects
        }
        first = false;

        file << "  {\n";
        file << "    \"Section Name\": \"" << stat->sectionName << "\",\n";
        file << "    \"Call Count\": " << stat->count << ",\n";
        file << "    \"Total Time\": " << stat->totalTime << ",\n";
        file << "    \"Min Time\": " << stat->minTime << ",\n";
        file << "    \"Max Time\": " << stat->maxTime << ",\n";
        file << "    \"Avg Time\": " << stat->avgTime << ",\n";
        file << "    \"File Name\": \"" << stat->fileName << "\",\n";
        file << "    \"Function Name\": \"" << stat->functionName << "\",\n";
        file << "    \"Line Number\": " << stat->lineNumber << "\n";
        file << "  }";
    }

    file << "\n]\n";  // End the JSON array

    file.close();
    std::cout << "Profiler stats written to " << fileName << " in JSON format.\n";
}

void Profiler::calculateStats() {
    // Iterate through all the recorded elapsed times
    for (const auto& elapsed : elapsedTimes) {
        const char* sectionName = elapsed.sectionName;

        // If section is not already in the stats map, create an entry for it
        if (stats.find(sectionName) == stats.end()) {
            stats[sectionName] = new ProfilerStats(sectionName);
        }

        // Retrieve the stats for the section
        ProfilerStats* sectionStats = stats[sectionName];

        // Update stats for the section
        sectionStats->count++;
        sectionStats->totalTime += elapsed.elapsedTime;
        sectionStats->minTime = std::min(sectionStats->minTime, elapsed.elapsedTime);
        sectionStats->maxTime = std::max(sectionStats->maxTime, elapsed.elapsedTime);
        sectionStats->avgTime = sectionStats->totalTime / sectionStats->count;

        // Update file name, function name, and line number (optional)
        sectionStats->fileName = elapsed.fileName;
        sectionStats->functionName = elapsed.functionName;
        sectionStats->lineNumber = elapsed.lineNumber;
    }
}

void Profiler::printStats() {
    // Iterate through the map of stats and output the details
    for (const auto& entry : stats) {
        ProfilerStats* stat = entry.second;
        std::cout << "Section Name: " << stat->sectionName << "\n";
        std::cout << "  Call Count: " << stat->count << "\n";
        std::cout << "  Total Time: " << stat->totalTime << " seconds\n";
        std::cout << "  Min Time: " << stat->minTime << " seconds\n";
        std::cout << "  Max Time: " << stat->maxTime << " seconds\n";
        std::cout << "  Avg Time: " << stat->avgTime << " seconds\n";
        std::cout << "  File Name: " << stat->fileName << "\n";
        std::cout << "  Function Name: " << stat->functionName << "\n";
        std::cout << "  Line Number: " << stat->lineNumber << "\n";
        std::cout << "\n";
    }
}


//memory management is slow, so we do pre allocation in Profiler()
