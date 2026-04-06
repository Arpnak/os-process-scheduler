#pragma once
#include "Scheduler.h"
#include <vector>

class Renderer {
public:
    // Print colored Gantt chart using ANSI codes
    static void printGantt(const std::string& algo_name,
                           const std::vector<GanttEntry>& gantt);

    // Print per-process stats table
    static void printProcessTable(const std::vector<Process>& results);

    // Print summary stats (avg WT, TAT, CPU util...)
    static void printStats(const Stats& stats);

    // Print comparison table across all algorithms
    static void printComparison(const std::vector<std::pair<std::string, Stats>>& results);

    // Separator line
    static void printSeparator(const std::string& c = "-", int width = 70);
};
