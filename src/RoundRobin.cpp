#include "Algorithms.h"
#include <algorithm>
#include <queue>

void RoundRobin::run(std::vector<Process> processes) {
    gantt_.clear();
    results_.clear();

    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrival_time < b.arrival_time;
              });

    std::queue<Process*> ready;
    int clock = 0;
    size_t idx = 0;
    int n = (int)processes.size();
    int completed = 0;

    // Seed first process
    if (!processes.empty()) {
        processes[0].state = ProcessState::READY;
        ready.push(&processes[0]);
        idx = 1;
    }

    while (completed < n) {
        if (ready.empty()) {
            // Idle gap
            int next = (int)processes[idx].arrival_time;
            gantt_.push_back({-1, clock, next});
            clock = next;
            while (idx < (size_t)n && processes[idx].arrival_time <= clock) {
                processes[idx].state = ProcessState::READY;
                ready.push(&processes[idx]);
                idx++;
            }
            continue;
        }

        Process* p = ready.front();
        ready.pop();

        if (!p->started) {
            p->start_time    = clock;
            p->response_time = clock - p->arrival_time;
            p->started       = true;
        }

        p->state = ProcessState::RUNNING;
        int slice = std::min(quantum_, p->remaining_time);
        int end   = clock + slice;

        gantt_.push_back({p->pid, clock, end});
        p->remaining_time -= slice;
        clock = end;

        // Enqueue processes that arrived during this slice
        while (idx < (size_t)n && processes[idx].arrival_time <= clock) {
            processes[idx].state = ProcessState::READY;
            ready.push(&processes[idx]);
            idx++;
        }

        if (p->remaining_time == 0) {
            p->finish_time = clock;
            p->state       = ProcessState::TERMINATED;
            results_.push_back(*p);
            completed++;
        } else {
            p->state = ProcessState::READY;
            ready.push(p);
        }
    }

    computeStats();
}
