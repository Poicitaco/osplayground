// sim/cpu/gantt.h
#pragma once
#include <string>
#include <vector>

struct GanttSlot {
    int pid;
    int start;
    int finish; // finish > start
};

std::string render_gantt_ascii(const std::vector<GanttSlot>& slots);
