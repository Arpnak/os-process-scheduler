#pragma once
#include <string>

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;          // lower number = higher priority
    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    bool started;
    ProcessState state;

    Process(int pid, int arrival, int burst, int priority = 0)
        : pid(pid), arrival_time(arrival), burst_time(burst),
          remaining_time(burst), priority(priority),
          start_time(-1), finish_time(-1),
          waiting_time(0), turnaround_time(0), response_time(-1),
          started(false), state(ProcessState::NEW) {}
};
