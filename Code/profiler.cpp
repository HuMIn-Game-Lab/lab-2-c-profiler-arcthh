#include "profiler.hpp"
#include "time.hpp" //should be time.hpp
#include <iostream>

//create a macro
#define PROFILER_ENTER(sectionName) Profiler::GetInstance()->EnterSection(sectionName);
#define PROFILER_EXIT(sectionName) Profiler::GetInstance()->(sectionName, __LINE__, __FILE__, __FUNCTION__);
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

    #if defined(DEBUG_PROFILER)
    //verify the stack isn't empty
    
    //verify the currentSection matches the sectionName
    #endif

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

//memory management is slow, so we do pre allocation in Profiler()
