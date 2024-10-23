#include "profiler.hpp"
#include "time.hpp"

//create a macro
#define PROFILER_ENTER(sectionName) Profiler::GetInstance()->EnterSection(sectionName);
#define PROFILER_EXIT(sectionName) Profiler::GetInstance()->ExitSection(sectionName, __LINE__, __FILE__, __FUNCTION__);

Profiler* Profiler::gProfiler = nullptr;

// Constructor for Time Record Start and Destructor
TimeRecordStart::TimeRecordStart(char const* sectionName, double secondsAtStart) : sectionName(sectionName), secondsAtStart(secondsAtStart) { };
TimeRecordStart::~TimeRecordStart() { };

// Constructor for Time Record Stop and Destructor
TimeRecordStop::TimeRecordStop(char const* sectionName, double secondsAtStop, int lineNumber, const char* fileName, const char* functionName ) : sectionName(sectionName), elapsedTime(secondsAtStop), lineNumber(lineNumber), fileName(fileName), functionName(functionName) { };
TimeRecordStop::TimeRecordStop(char const* sectionName, double secondsAtStop ) : sectionName(sectionName), elapsedTime(secondsAtStop), lineNumber(0), fileName("null"), functionName("null") { };
TimeRecordStop::~TimeRecordStop() { };

// Constructor for ProfilerScopeObject and Destructor
ProfilerScopeObject::ProfilerScopeObject(char const* sectionName) : sectionName(sectionName) {
    Profiler::GetInstance()->EnterSection(sectionName);
}
ProfilerScopeObject::~ProfilerScopeObject() {
    Profiler::GetInstance()->ExitSection(sectionName);
}

// Constructor for ProfilerStats and Destructor
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
// Destructor for ProfilerStats (no dynamic memory to clean up here)
ProfilerStats::~ProfilerStats() {}

// Profiler constructor: Initializes the Profiler instance and reserves space for elapsed times
Profiler::Profiler() {
    gProfiler = this;
    elapsedTimes.reserve(1000000); //Pre-allocate memory to store profiling data
}

// Profiler singleton: Creates or retrieves the single instance of the Profiler
Profiler* Profiler::GetInstance() {
    if(gProfiler == nullptr) {
        gProfiler = new Profiler();
    }
    return gProfiler;
}
// Destructor for Profiler (could clean up dynamic resources here if necessary)
Profiler::~Profiler() {}   

// EnterSection: Records the start time for a section and stores it in the section map
void Profiler::EnterSection(char const* sectionName) {
    double secondsAtStart = GetCurrentTimeSeconds();
    sectionMap.emplace(sectionName, TimeRecordStart(sectionName, secondsAtStart));
}

// ExitSection: Overloaded function to support simple section exit (delegates to full exit function with additional details)
void Profiler::ExitSection(char const* sectionName) {
    double secondsAtStop = GetCurrentTimeSeconds();    
}

// ExitSection (overloaded): Completes profiling for a section, calculates elapsed time, and reports the data
void Profiler::ExitSection(char const* sectionName, int lineNumber, const char* fileName, const char* functionName) {
    double secondsAtStop = GetCurrentTimeSeconds();

    // Find the section in the map to retrieve its start time
    auto it = sectionMap.find(sectionName);

    if (it != sectionMap.end()) {
        TimeRecordStart sectionStart = it->second;

        // Calculate the elapsed time
        double elapsedTime = secondsAtStop - sectionStart.secondsAtStart;

        // Remove the section from the map
        sectionMap.erase(it);

        // Report the time spent in this section
        ReportSectionTime(sectionName, elapsedTime, lineNumber, fileName, functionName);
    } else {
        std::cerr << "Error: Mismatched section exit for " << sectionName << std::endl;
    }
}

// ReportSectionTime: Updates the statistics for a given section based on its elapsed time
void Profiler::ReportSectionTime(char const* sectionName, double elapsedTime, int lineNumber, const char* fileName, const char* functionName) {
    // Check if the section already exists in the stats map
    if (stats.find(sectionName) == stats.end()) {
        stats[sectionName] = new ProfilerStats(sectionName);  // Create a new stats entry if it doesn't exist
    }

    // Update the stats for the section
    ProfilerStats* sectionStats = stats[sectionName];
    sectionStats->count++;  // Increment the count of calls
    sectionStats->totalTime += elapsedTime;  // Add to the total time
    sectionStats->minTime = std::min(sectionStats->minTime, elapsedTime);  // Update minimum time
    sectionStats->maxTime = std::max(sectionStats->maxTime, elapsedTime);  // Update maximum time
    sectionStats->avgTime = sectionStats->totalTime / sectionStats->count;  // Calculate average time

    // Update additional metadata for debugging purposes
    sectionStats->fileName = fileName;
    sectionStats->functionName = functionName;
    sectionStats->lineNumber = lineNumber;
}


// printStatsToCSV: Writes the profiling statistics to a CSV file
void Profiler::printStatsToCSV(const char* fileName) {
    std::ofstream file(fileName);  // Open the file

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open file for CSV output." << std::endl;
        return;
    }

    // Write the CSV headers
    file << "Section Name, Call Count, Total Time, Min Time, Max Time, Avg Time, File Name, Function Name, Line Number\n";

    // Write each section's statistics to the CSV
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

// printStatsToJSON: Writes the profiling statistics to a JSON file
void Profiler::printStatsToJSON(const char* fileName) {
    std::ofstream file(fileName);  // Open the file

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

        // Write the JSON object for each section
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

// calculateStats: Updates the statistics based on recorded elapsed times
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

// printStats: Outputs the profiling statistics to the console
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
