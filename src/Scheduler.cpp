#include "Scheduler.h"
#include <numeric>

void Scheduler::computeStats() {
    if (results_.empty()) return;

    double total_wt = 0, total_tat = 0, total_rt = 0;
    int busy = 0;

    for (auto& p : results_) {
        p.turnaround_time = p.finish_time - p.arrival_time;
        p.waiting_time    = p.turnaround_time - p.burst_time;
        total_wt  += p.waiting_time;
        total_tat += p.turnaround_time;
        total_rt  += p.response_time;
        busy      += p.burst_time;
    }

    int total_time = 0;
    for (auto& g : gantt_) total_time = std::max(total_time, g.end);

    int n = (int)results_.size();
    stats_.avg_waiting_time    = total_wt  / n;
    stats_.avg_turnaround_time = total_tat / n;
    stats_.avg_response_time   = total_rt  / n;
    stats_.cpu_utilization     = total_time > 0 ? 100.0 * busy / total_time : 0;
    stats_.throughput          = total_time > 0 ? (double)n / total_time : 0;
    stats_.total_time          = total_time;
}
