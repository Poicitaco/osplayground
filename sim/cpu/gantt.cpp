// sim/cpu/gantt.cpp
#include "gantt.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

std::string render_gantt_ascii(const std::vector<GanttSlot>& slots_in) {
    if (slots_in.empty()) return "[empty]";

    std::vector<GanttSlot> slots;
    slots.reserve(slots_in.size());
    for (auto s : slots_in) {
        if (!slots.empty() && slots.back().pid == s.pid && slots.back().finish == s.start) {
            slots.back().finish = s.finish;
        } else {
            slots.push_back(s);
        }
    }

    std::ostringstream out;

    // Border trên
    out << ' ';
    for (auto &s : slots) {
        int w = std::max(1, s.finish - s.start);
        out << '+' << std::string(w * 2, '-');
    }
    out << "+\n|";

    // Hàng PID
    for (auto &s : slots) {
        int w = std::max(1, s.finish - s.start);
        std::string txt = "P" + std::to_string(s.pid);
        int cell = w * 2;
        int padl = std::max(0, (cell - (int)txt.size()) / 2);
        int padr = std::max(0, cell - padl - (int)txt.size());
        out << std::string(padl, ' ') << txt << std::string(padr, ' ') << '|';
    }
    out << "\n ";

    // Border dưới
    for (auto &s : slots) {
        int w = std::max(1, s.finish - s.start);
        out << '+' << std::string(w * 2, '-');
    }
    out << "+\n";

    // Thời gian
    int t = slots.front().start;
    out << std::setw(2) << t;
    for (auto &s : slots) {
        int w = std::max(1, s.finish - s.start);
        t = s.finish;
        out << std::string(w * 2 - 1, ' ') << std::setw(3) << t;
    }
    out << "\n";
    return out.str();
}
