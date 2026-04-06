#include "Algorithms.h"
#include <algorithm>
#include <queue>

void FCFS::run(std::vector<Process> processes) {
    gantt_.clear();
    results_.clear();

    // Sort by arrival time
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrival_time < b.arrival_time;
              });

    int clock = 0;
    std::queue<Process*> ready;
    size_t idx = 0;

    while (idx < processes.size() || !ready.empty()) {
        // Enqueue all processes that have arrived
        while (idx < processes.size() && processes[idx].arrival_time <= clock) {
            processes[idx].state = ProcessState::READY;
            ready.push(&processes[idx]);
            idx++;
        }

        if (ready.empty()) {
            // CPU idle — jump to next arrival
            int next = processes[idx].arrival_time;
            gantt_.push_back({-1, clock, next});
            clock = next;
            continue;
        }

        Process* p = ready.front();
        ready.pop();

        p->start_time   = clock;
        p->response_time = clock - p->arrival_time;
        p->state        = ProcessState::RUNNING;

        gantt_.push_back({p->pid, clock, clock + p->burst_time});

        clock          += p->burst_time;
        p->finish_time  = clock;
        p->state        = ProcessState::TERMINATED;
        results_.push_back(*p);
    }

    computeStats();
}
