// sim/cpu/scheduler.h
#pragma once
#include <vector>

struct Process {
    int pid;
    int arrival;
    int burst;
    int priority = 0;   // nhỏ = ưu tiên cao

    int start = 0;
    int finish = 0;
    int waiting = 0;
    int turnaround = 0;
};

struct Result {
    double awt = 0.0;
    double att = 0.0;
};

class Scheduler {
public:
    Result runFCFS(std::vector<Process> procs);
    Result runSJF(std::vector<Process> procs);               // non-preemptive
    Result runSRTF(std::vector<Process> procs);              // SJF preemptive
    Result runRR(std::vector<Process> procs, int quantum);   // preemptive
    Result runPriorityNP(std::vector<Process> procs);        // non-preemptive
    Result runPriorityP (std::vector<Process> procs);        // preemptive
};
