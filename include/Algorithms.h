#pragma once
#include "Scheduler.h"

// ─── First Come First Served ───────────────────────────────────────────────
class FCFS : public Scheduler {
public:
    std::string name() const override { return "FCFS"; }
    void run(std::vector<Process> processes) override;
};

// ─── Shortest Job First (Preemptive / SRTF) ────────────────────────────────
class SJF : public Scheduler {
public:
    std::string name() const override { return "SJF (Preemptive)"; }
    void run(std::vector<Process> processes) override;
};

// ─── Round Robin ──────────────────────────────────────────────────────────
class RoundRobin : public Scheduler {
public:
    explicit RoundRobin(int quantum = 2) : quantum_(quantum) {}
    std::string name() const override { return "Round Robin (q=" + std::to_string(quantum_) + ")"; }
    void run(std::vector<Process> processes) override;
private:
    int quantum_;
};

// ─── Priority Scheduling with Aging ────────────────────────────────────────
class PriorityScheduler : public Scheduler {
public:
    explicit PriorityScheduler(int aging_interval = 5)
        : aging_interval_(aging_interval) {}
    std::string name() const override { return "Priority + Aging"; }
    void run(std::vector<Process> processes) override;
private:
    int aging_interval_;
};
