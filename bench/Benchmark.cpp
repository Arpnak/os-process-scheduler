#include <algorithm>
#include "Algorithms.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>

namespace C {
    const char* R  = "\033[0m";
    const char* B  = "\033[1m";
    const char* CY = "\033[96m";
    const char* GR = "\033[92m";
    const char* YE = "\033[93m";
    const char* DM = "\033[2m";
}

// Naive O(n) SJF: linear scan every tick — no heap
class SJF_Naive : public Scheduler {
public:
    std::string name() const override { return "SJF Naive O(n)"; }
    void run(std::vector<Process> processes) override {
        gantt_.clear(); results_.clear();
        std::sort(processes.begin(), processes.end(),
                  [](const Process& a, const Process& b){
                      return a.arrival_time < b.arrival_time; });
        int clock = 0, completed = 0, n = (int)processes.size();
        while (completed < n) {
            Process* best = nullptr;
            for (auto& p : processes)
                if (p.arrival_time <= clock && p.state != ProcessState::TERMINATED)
                    if (!best || p.remaining_time < best->remaining_time)
                        best = &p;
            if (!best) { clock++; continue; }
            if (!best->started) {
                best->start_time    = clock;
                best->response_time = clock - best->arrival_time;
                best->started       = true;
            }
            best->remaining_time--;
            clock++;
            if (best->remaining_time == 0) {
                best->finish_time = clock;
                best->state       = ProcessState::TERMINATED;
                results_.push_back(*best);
                completed++;
            }
        }
        computeStats();
    }
};

static std::vector<Process> generate(int n) {
    std::vector<Process> procs;
    int arrival = 0;
    for (int i = 1; i <= n; i++) {
        arrival += rand() % 3;
        procs.emplace_back(i, arrival, 1 + rand() % 20, rand() % 10);
    }
    return procs;
}

static long long timeUs(Scheduler& sched, const std::vector<Process>& procs, int reps) {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < reps; i++) sched.run(procs);
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

int main() {
    std::srand((unsigned)std::time(nullptr));

    std::cout << C::CY << C::B
<< "\n"
<< "  +==========================================+\n"
<< "  |   Heap O(log n) vs Naive O(n) Benchmark  |\n"
<< "  +==========================================+\n"
<< C::R << "\n";

    std::cout << C::B
              << std::left  << std::setw(10) << "  N"
              << std::right << std::setw(18) << "Naive (us)"
              << std::setw(18) << "Heap  (us)"
              << std::setw(12) << "Speedup"
              << C::R << "\n";
    for (int i = 0; i < 60; i++) std::cout << "-";
    std::cout << "\n";

    for (int n : {100, 500, 1000, 3000, 5000, 10000}) {
        auto procs = generate(n);
        SJF_Naive naive;
        SJF       heap;
        int reps = (n <= 1000) ? 10 : 3;
        long long t_naive = timeUs(naive, procs, reps) / reps;
        long long t_heap  = timeUs(heap,  procs, reps) / reps;
        double speedup = t_heap > 0 ? (double)t_naive / t_heap : 0;

        std::cout << "  " << std::left  << std::setw(8)  << n
                  << std::right << std::setw(18) << t_naive
                  << std::setw(18) << t_heap
                  << std::setw(10) << std::fixed << std::setprecision(2) << speedup << "x";
        if (speedup > 1.5) std::cout << "  " << C::GR << "<-- faster" << C::R;
        std::cout << "\n";
    }

    std::cout << "\n" << C::YE
              << "  Heap dispatch: O(log n) per preemption decision\n"
              << "  Naive dispatch: O(n) linear scan per clock tick\n"
              << C::R << "\n";
    return 0;
}
