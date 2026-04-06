#include "Algorithms.h"
#include <algorithm>
#include <queue>
#include <vector>

void PriorityScheduler::run(std::vector<Process> processes) {
    gantt_.clear();
    results_.clear();

    // Effective priority = priority - age_bonus (lower = better)
    // Aging: every `aging_interval_` ticks in ready queue, priority improves by 1
    struct Entry {
        Process* proc;
        int      effective_priority;
        int      wait_since; // clock when it entered ready queue
    };

    auto cmp = [](const Entry& a, const Entry& b) {
        if (a.effective_priority != b.effective_priority)
            return a.effective_priority > b.effective_priority;
        return a.proc->arrival_time > b.proc->arrival_time;
    };
    std::priority_queue<Entry, std::vector<Entry>, decltype(cmp)> pq(cmp);

    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrival_time < b.arrival_time;
              });

    int clock = 0, idx = 0, completed = 0;
    int n = (int)processes.size();

    while (completed < n) {
        // Enqueue arrivals
        while (idx < n && processes[idx].arrival_time <= clock) {
            pq.push({&processes[idx], processes[idx].priority, clock});
            processes[idx].state = ProcessState::READY;
            idx++;
        }

        if (pq.empty()) {
            int next = processes[idx].arrival_time;
            gantt_.push_back({-1, clock, next});
            clock = next;
            continue;
        }

        // Rebuild heap with updated aging
        std::vector<Entry> tmp;
        while (!pq.empty()) { tmp.push_back(pq.top()); pq.pop(); }
        for (auto& e : tmp) {
            int age_bonus = (clock - e.wait_since) / aging_interval_;
            e.effective_priority = std::max(0, e.proc->priority - age_bonus);
            pq.push(e);
        }

        Entry top = pq.top(); pq.pop();
        Process* p = top.proc;

        if (!p->started) {
            p->start_time    = clock;
            p->response_time = clock - p->arrival_time;
            p->started       = true;
        }

        p->state = ProcessState::RUNNING;

        // Run for 1 tick (preemptive priority)
        int next_event = clock + p->remaining_time;
        if (idx < n) next_event = std::min(next_event, processes[idx].arrival_time);
        int slice = next_event - clock;

        if (!gantt_.empty() && gantt_.back().pid == p->pid && gantt_.back().end == clock)
            gantt_.back().end += slice;
        else
            gantt_.push_back({p->pid, clock, clock + slice});

        p->remaining_time -= slice;
        clock += slice;

        if (p->remaining_time == 0) {
            p->finish_time = clock;
            p->state       = ProcessState::TERMINATED;
            results_.push_back(*p);
            completed++;
        } else {
            p->state = ProcessState::READY;
            pq.push({p, p->priority, clock}); // re-queue with reset wait time
        }
    }

    computeStats();
}
