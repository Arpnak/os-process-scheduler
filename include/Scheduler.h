#pragma once
#include "Process.h"
#include <vector>
#include <string>

struct GanttEntry {
    int pid;       // -1 = idle
    int start;
    int end;
};

struct Stats {
    double avg_waiting_time;
    double avg_turnaround_time;
    double avg_response_time;
    double cpu_utilization;
    double throughput;
    int total_time;
};

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual std::string name() const = 0;

    // Run the full simulation and populate gantt + stats
    virtual void run(std::vector<Process> processes) = 0;

    const std::vector<GanttEntry>& gantt() const { return gantt_; }
    const std::vector<Process>&    results() const { return results_; }
    const Stats&                   stats()   const { return stats_; }

protected:
    std::vector<GanttEntry> gantt_;
    std::vector<Process>    results_;
    Stats                   stats_{};

    void computeStats();
};
