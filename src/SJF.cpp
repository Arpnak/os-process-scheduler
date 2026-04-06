#include "Algorithms.h"
#include <algorithm>
#include <queue>

void SJF::run(std::vector<Process> processes) {
    gantt_.clear();
    results_.clear();

    // Min-heap: shortest remaining time first
    // This is O(log n) per dispatch vs O(n) for naive linear scan
    auto cmp = [](Process* a, Process* b) {
        if (a->remaining_time != b->remaining_time)
            return a->remaining_time > b->remaining_time;
        return a->arrival_time > b->arrival_time;
    };
    std::priority_queue<Process*, std::vector<Process*>, decltype(cmp)> pq(cmp);

    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrival_time < b.arrival_time;
              });

    int clock = 0;
    size_t idx = 0;
    int completed = 0;
    int n = (int)processes.size();

    while (completed < n) {
        while (idx < (size_t)n && processes[idx].arrival_time <= clock) {
            processes[idx].state = ProcessState::READY;
            pq.push(&processes[idx]);
            idx++;
        }

        if (pq.empty()) {
            int next = (int)processes[idx].arrival_time;
            gantt_.push_back({-1, clock, next});
            clock = next;
            continue;
        }

        Process* p = pq.top(); pq.pop();

        if (!p->started) {
            p->start_time    = clock;
            p->response_time = clock - p->arrival_time;
            p->started       = true;
        }

        p->state = ProcessState::RUNNING;

        // Find when next process arrives (preemption point)
        int run_until = clock + p->remaining_time;
        if (idx < (size_t)n)
            run_until = std::min(run_until, (int)processes[idx].arrival_time);

        int slice = run_until - clock;

        // Merge consecutive slices for same process in Gantt
        if (!gantt_.empty() && gantt_.back().pid == p->pid && gantt_.back().end == clock)
            gantt_.back().end = run_until;
        else
            gantt_.push_back({p->pid, clock, run_until});

        p->remaining_time -= slice;
        clock = run_until;

        if (p->remaining_time == 0) {
            p->finish_time = clock;
            p->state       = ProcessState::TERMINATED;
            results_.push_back(*p);
            completed++;
        } else {
            p->state = ProcessState::READY;
            pq.push(p);
        }

        // Enqueue newly arrived
        while (idx < (size_t)n && processes[idx].arrival_time <= clock) {
            processes[idx].state = ProcessState::READY;
            pq.push(&processes[idx]);
            idx++;
        }
    }

    computeStats();
}
