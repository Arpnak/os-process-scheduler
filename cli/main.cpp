#include "Algorithms.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <memory>

namespace C {
    const char* R  = "\033[0m";
    const char* B  = "\033[1m";
    const char* CY = "\033[96m";
    const char* YE = "\033[93m";
    const char* GR = "\033[92m";
    const char* DM = "\033[2m";
}

static void printBanner() {
    std::cout << C::CY << C::B
<< "\n"
<< "  +===================================================+\n"
<< "  |      OS Process Scheduler Simulator v1.0          |\n"
<< "  |      FCFS  *  SJF  *  Round Robin  *  Priority    |\n"
<< "  +===================================================+\n"
<< C::R << "\n";
}

static void printUsage() {
    std::cout
        << C::B << "Usage:\n" << C::R
        << "  ./scheduler --algo <fcfs|sjf|rr|priority|all> [options]\n\n"
        << C::B << "Options:\n" << C::R
        << "  --file   <path>   Load processes from CSV\n"
        << "  --random <n>      Generate n random processes\n"
        << "  --quantum <q>     Time quantum for RR (default 2)\n"
        << "  --aging  <a>      Aging interval for Priority (default 5)\n\n"
        << C::B << "CSV format (no header):\n" << C::R
        << "  pid,arrival_time,burst_time,priority\n\n"
        << C::B << "Examples:\n" << C::R
        << "  ./scheduler --algo all --random 6\n"
        << "  ./scheduler --algo rr --quantum 3 --file procs.csv\n"
        << "  ./scheduler --algo sjf --random 8\n\n";
}

static std::string getArg(const std::vector<std::string>& args,
                          const std::string& flag, const std::string& def = "") {
    for (size_t i = 0; i + 1 < args.size(); i++)
        if (args[i] == flag) return args[i + 1];
    return def;
}

static bool hasFlag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

static std::vector<Process> loadFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Error: cannot open file: " << path << "\n";
        exit(1);
    }
    std::vector<Process> procs;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string tok;
        std::vector<int> vals;
        while (std::getline(ss, tok, ',')) vals.push_back(std::stoi(tok));
        if (vals.size() >= 3)
            procs.emplace_back(vals[0], vals[1], vals[2],
                               vals.size() >= 4 ? vals[3] : 0);
    }
    return procs;
}

static std::vector<Process> generateRandom(int n) {
    std::vector<Process> procs;
    int arrival = 0;
    for (int i = 1; i <= n; i++) {
        arrival += rand() % 4;
        int burst    = 1 + rand() % 10;
        int priority = rand() % 5;
        procs.emplace_back(i, arrival, burst, priority);
    }
    return procs;
}

static void printProcesses(const std::vector<Process>& procs) {
    std::cout << C::B << "  Processes:\n" << C::R;
    std::cout << C::DM << "  PID  Arrival  Burst  Priority\n" << C::R;
    for (auto& p : procs)
        std::cout << "  " << std::setw(3) << p.pid
                  << std::setw(8)  << p.arrival_time
                  << std::setw(7)  << p.burst_time
                  << std::setw(9)  << p.priority << "\n";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::srand((unsigned)std::time(nullptr));
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.empty() || hasFlag(args, "--help") || hasFlag(args, "-h")) {
        printBanner(); printUsage(); return 0;
    }

    printBanner();

    std::string algo    = getArg(args, "--algo", "all");
    int         quantum = std::stoi(getArg(args, "--quantum", "2"));
    int         aging   = std::stoi(getArg(args, "--aging",   "5"));

    std::vector<Process> procs;
    if (hasFlag(args, "--file")) {
        procs = loadFromFile(getArg(args, "--file"));
    } else if (hasFlag(args, "--random")) {
        procs = generateRandom(std::stoi(getArg(args, "--random", "5")));
    } else {
        procs = {
            {1, 0, 8, 2}, {2, 1, 4, 1}, {3, 2, 9, 3},
            {4, 3, 5, 0}, {5, 4, 2, 1},
        };
        std::cout << C::YE << "  No input given -- using built-in demo processes.\n\n" << C::R;
    }

    printProcesses(procs);

    std::vector<std::unique_ptr<Scheduler>> schedulers;
    if (algo == "all" || algo == "fcfs")     schedulers.push_back(std::make_unique<FCFS>());
    if (algo == "all" || algo == "sjf")      schedulers.push_back(std::make_unique<SJF>());
    if (algo == "all" || algo == "rr")       schedulers.push_back(std::make_unique<RoundRobin>(quantum));
    if (algo == "all" || algo == "priority") schedulers.push_back(std::make_unique<PriorityScheduler>(aging));

    if (schedulers.empty()) {
        std::cerr << "Unknown algorithm: " << algo << "\n";
        printUsage(); return 1;
    }

    std::vector<std::pair<std::string, Stats>> comparison;
    for (auto& sched : schedulers) {
        sched->run(procs);
        Renderer::printGantt(sched->name(), sched->gantt());
        Renderer::printProcessTable(sched->results());
        Renderer::printStats(sched->stats());
        comparison.push_back({sched->name(), sched->stats()});
        Renderer::printSeparator("=", 70);
        std::cout << "\n";
    }

    if (comparison.size() > 1)
        Renderer::printComparison(comparison);

    return 0;
}
