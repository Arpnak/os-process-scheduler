#include "Renderer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>

namespace Color {
    const char* RESET   = "\033[0m";
    const char* BOLD    = "\033[1m";
    const char* DIM     = "\033[2m";
    const char* WHITE   = "\033[97m";
    const char* CYAN    = "\033[96m";
    const char* GREEN   = "\033[92m";
    const char* YELLOW  = "\033[93m";
    const char* BG[] = {
        "\033[41m", "\033[42m", "\033[44m",
        "\033[45m", "\033[46m", "\033[43m",
    };
    const char* BG_IDLE = "\033[100m";
}

static std::string centerText(const std::string& s, int width) {
    int pad = width - (int)s.size();
    if (pad <= 0) return s.substr(0, width);
    int left = pad / 2, right = pad - left;
    return std::string(left, ' ') + s + std::string(right, ' ');
}

void Renderer::printSeparator(const std::string& c, int width) {
    std::cout << Color::DIM;
    for (int i = 0; i < width; i++) std::cout << c;
    std::cout << Color::RESET << "\n";
}

void Renderer::printGantt(const std::string& algo_name,
                          const std::vector<GanttEntry>& gantt) {
    std::cout << "\n" << Color::BOLD << Color::CYAN
              << "  *** " << algo_name << " -- Gantt Chart ***"
              << Color::RESET << "\n\n";
    if (gantt.empty()) { std::cout << "  (empty)\n\n"; return; }

    int total_time = 0;
    for (auto& g : gantt) total_time = std::max(total_time, g.end);
    double scale = (total_time > 58) ? 58.0 / total_time : 1.0;

    std::cout << "  ";
    for (auto& g : gantt) {
        int w = std::max(2, (int)std::round((g.end - g.start) * scale));
        if (g.pid == -1) {
            std::cout << Color::BG_IDLE << Color::WHITE
                      << centerText("idle", w) << Color::RESET;
        } else {
            std::cout << Color::BG[g.pid % 6] << Color::WHITE << Color::BOLD
                      << centerText("P" + std::to_string(g.pid), w)
                      << Color::RESET;
        }
    }
    std::cout << "\n  ";
    for (auto& g : gantt) {
        int w = std::max(2, (int)std::round((g.end - g.start) * scale));
        std::string ts = std::to_string(g.start);
        std::cout << Color::DIM << ts;
        int gap = w - (int)ts.size();
        if (gap > 0) std::cout << std::string(gap, ' ');
        std::cout << Color::RESET;
    }
    std::cout << Color::DIM << total_time << Color::RESET << "\n\n";
}

void Renderer::printProcessTable(const std::vector<Process>& results) {
    std::cout << Color::BOLD << Color::WHITE
              << "  PID  Arrival  Burst  Start  Finish  Waiting  Turnaround  Response\n"
              << Color::RESET;
    printSeparator("-", 70);

    std::vector<Process> sorted = results;
    std::sort(sorted.begin(), sorted.end(),
              [](const Process& a, const Process& b) { return a.pid < b.pid; });

    for (auto& p : sorted) {
        std::cout
            << "  "
            << Color::CYAN  << std::setw(3)  << p.pid            << Color::RESET
            << "  "         << std::setw(7)  << p.arrival_time
            << "  "         << std::setw(5)  << p.burst_time
            << "  "         << std::setw(5)  << p.start_time
            << "  "         << std::setw(6)  << p.finish_time
            << "  " << Color::YELLOW << std::setw(7) << p.waiting_time    << Color::RESET
            << "  " << Color::GREEN  << std::setw(10)<< p.turnaround_time << Color::RESET
            << "  "         << std::setw(8)  << p.response_time
            << "\n";
    }
    std::cout << "\n";
}

void Renderer::printStats(const Stats& stats) {
    auto bar = [](double pct, int w = 30) -> std::string {
        int filled = (int)(pct / 100.0 * w);
        std::string s = "[";
        for (int i = 0; i < w; i++) s += (i < filled ? "#" : ".");
        return s + "]";
    };

    std::cout << Color::BOLD << "  Summary\n" << Color::RESET;
    printSeparator("-", 70);
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Avg Waiting Time    : "
              << Color::YELLOW << std::setw(6) << stats.avg_waiting_time  << Color::RESET << " ticks\n";
    std::cout << "  Avg Turnaround Time : "
              << Color::GREEN  << std::setw(6) << stats.avg_turnaround_time << Color::RESET << " ticks\n";
    std::cout << "  Avg Response Time   : "
              << std::setw(6)  << stats.avg_response_time << " ticks\n";
    std::cout << "  CPU Utilization     : "
              << Color::CYAN   << bar(stats.cpu_utilization)
              << " " << stats.cpu_utilization << "%" << Color::RESET << "\n";
    std::cout << "  Throughput          : "
              << std::setprecision(4) << stats.throughput << " proc/tick\n\n";
}

void Renderer::printComparison(
        const std::vector<std::pair<std::string, Stats>>& data) {

    std::cout << "\n" << Color::BOLD << Color::WHITE
              << "======================= ALGORITHM COMPARISON =======================\n"
              << Color::RESET;

    std::cout << Color::BOLD
              << std::left  << std::setw(24) << "  Algorithm"
              << std::right << std::setw(9)  << "Avg WT"
              << std::setw(10) << "Avg TAT"
              << std::setw(9)  << "Avg RT"
              << std::setw(11) << "CPU Util%"
              << std::setw(12) << "Throughput"
              << Color::RESET << "\n";
    printSeparator("-", 76);

    double best_wt = 1e9, best_tat = 1e9, best_util = 0;
    for (auto& [nm, s] : data) {
        best_wt   = std::min(best_wt,  s.avg_waiting_time);
        best_tat  = std::min(best_tat, s.avg_turnaround_time);
        best_util = std::max(best_util, s.cpu_utilization);
    }

    for (auto& [nm, s] : data) {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  " << std::left << std::setw(22) << nm << std::right;

        if (s.avg_waiting_time    <= best_wt   + 0.01) std::cout << Color::GREEN << Color::BOLD;
        std::cout << std::setw(9)  << s.avg_waiting_time    << Color::RESET;
        if (s.avg_turnaround_time <= best_tat  + 0.01) std::cout << Color::GREEN << Color::BOLD;
        std::cout << std::setw(10) << s.avg_turnaround_time << Color::RESET;
        std::cout << std::setw(9)  << s.avg_response_time;
        if (s.cpu_utilization     >= best_util - 0.01) std::cout << Color::GREEN << Color::BOLD;
        std::cout << std::setw(10) << s.cpu_utilization << "%" << Color::RESET;
        std::cout << std::setw(11) << std::setprecision(4) << s.throughput << "\n";
    }

    printSeparator("=", 76);
    std::cout << Color::DIM << "  "
              << Color::GREEN << "Green+Bold" << Color::DIM
              << " = best in that column\n\n" << Color::RESET;
}
