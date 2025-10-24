// sim/cpu/scheduler.cpp
#include "scheduler.h"
#include "gantt.h"
#include <algorithm>
#include <iostream>
#include <queue>

static Result summarize(const std::vector<Process>& ps) {
    long long sw = 0, st = 0;
    for (auto &p : ps) { sw += p.waiting; st += p.turnaround; }
    Result r;
    if (!ps.empty()) {
        r.awt = (double)sw / ps.size();
        r.att = (double)st / ps.size();
    }
    return r;
}

// FCFS
Result Scheduler::runFCFS(std::vector<Process> procs) {
    std::sort(procs.begin(), procs.end(),
              [](const Process& a, const Process& b){
                  if (a.arrival != b.arrival) return a.arrival < b.arrival;
                  return a.pid < b.pid;
              });
    int time = 0;
    std::vector<GanttSlot> gantt;

    std::cout << "== FCFS ==\nPID\tArr\tBurst\tStart\tFinish\tWait\tTurn\n";
    for (auto &p : procs) {
        if (time < p.arrival) time = p.arrival;
        p.start = time;
        p.finish = time + p.burst;
        p.waiting = p.start - p.arrival;
        p.turnaround = p.finish - p.arrival;
        time = p.finish;

        gantt.push_back({p.pid, p.start, p.finish});
        std::cout << p.pid << '\t' << p.arrival << '\t' << p.burst << '\t'
                  << p.start << '\t' << p.finish << '\t'
                  << p.waiting << '\t' << p.turnaround << '\n';
    }
    auto r = summarize(procs);
    std::cout << "AWT=" << r.awt << " ATT=" << r.att << "\n";
    std::cout << render_gantt_ascii(gantt) << "\n";
    return r;
}

// SJF non-preemptive
Result Scheduler::runSJF(std::vector<Process> procs) {
    std::sort(procs.begin(), procs.end(),
              [](const Process& a, const Process& b){
                  if (a.arrival != b.arrival) return a.arrival < b.arrival;
                  return a.burst < b.burst;
              });

    int time = 0, done = 0, n = (int)procs.size();
    std::vector<int> used(n, 0);
    std::vector<GanttSlot> gantt;

    std::cout << "== SJF (non-preemptive) ==\nPID\tArr\tBurst\tStart\tFinish\tWait\tTurn\n";
    while (done < n) {
        int idx = -1, bestBurst = 1e9;
        for (int i = 0; i < n; ++i) {
            if (used[i]) continue;
            if (procs[i].arrival <= time && procs[i].burst < bestBurst) {
                bestBurst = procs[i].burst; idx = i;
            }
        }
        if (idx == -1) { time++; continue; }

        auto &p = procs[idx];
        p.start = time;
        p.finish = time + p.burst;
        p.waiting = p.start - p.arrival;
        p.turnaround = p.finish - p.arrival;
        time = p.finish;
        used[idx] = 1; done++;

        gantt.push_back({p.pid, p.start, p.finish});
        std::cout << p.pid << '\t' << p.arrival << '\t' << p.burst << '\t'
                  << p.start << '\t' << p.finish << '\t'
                  << p.waiting << '\t' << p.turnaround << '\n';
    }
    auto r = summarize(procs);
    std::cout << "AWT=" << r.awt << " ATT=" << r.att << "\n";
    std::cout << render_gantt_ascii(gantt) << "\n";
    return r;
}

// SRTF (preemptive SJF)
Result Scheduler::runSRTF(std::vector<Process> procs) {
    int n = (int)procs.size();
    std::sort(procs.begin(), procs.end(),
              [](const Process& a, const Process& b){
                  if (a.arrival != b.arrival) return a.arrival < b.arrival;
                  return a.pid < b.pid;
              });
    std::vector<int> rt(n);
    for (int i = 0; i < n; ++i) rt[i] = procs[i].burst;

    int time = procs[0].arrival;
    int done = 0, cur = -1, curStart = time;
    std::vector<int> started(n, 0);
    std::vector<GanttSlot> gantt;

    auto pick = [&](int t)->int {
        int best = -1, bestRt = 1e9;
        for (int i = 0; i < n; ++i) {
            if (procs[i].arrival <= t && rt[i] > 0) {
                if (rt[i] < bestRt) { bestRt = rt[i]; best = i; }
            }
        }
        return best;
    };

    std::cout << "== SRTF (preemptive SJF) ==\nPID\tArr\tBurst\tStart\tFinish\tWait\tTurn\n";
    while (done < n) {
        int candidate = pick(time);
        if (candidate == -1) { time++; continue; }

        if (candidate != cur) {
            if (cur != -1) gantt.push_back({procs[cur].pid, curStart, time});
            cur = candidate; curStart = time;
            if (!started[cur]) { started[cur] = 1; procs[cur].start = time; }
        }

        rt[cur]--; time++;

        if (rt[cur] == 0) {
            gantt.push_back({procs[cur].pid, curStart, time});
            procs[cur].finish = time;
            procs[cur].turnaround = procs[cur].finish - procs[cur].arrival;
            procs[cur].waiting   = procs[cur].turnaround - procs[cur].burst;
            done++; cur = -1;
        }
    }

    for (auto &p : procs) {
        std::cout << p.pid << '\t' << p.arrival << '\t' << p.burst << '\t'
                  << p.start << '\t' << p.finish << '\t'
                  << p.waiting << '\t' << p.turnaround << '\n';
    }
    auto r = summarize(procs);
    std::cout << "AWT=" << r.awt << " ATT=" << r.att << "\n";
    std::cout << render_gantt_ascii(gantt) << "\n";
    return r;
}

// RR (preemptive) – bản fix
Result Scheduler::runRR(std::vector<Process> procs, int quantum) {
    int n = (int)procs.size();
    std::sort(procs.begin(), procs.end(),
              [](const Process& a, const Process& b){
                  if (a.arrival != b.arrival) return a.arrival < b.arrival;
                  return a.pid < b.pid;
              });

    std::vector<int> rt(n);
    for (int i = 0; i < n; ++i) rt[i] = procs[i].burst;

    int time = std::max(0, procs[0].arrival);
    int done = 0, nextIdx = 0;
    std::queue<int> q;
    std::vector<int> started(n, 0);
    std::vector<GanttSlot> gantt;

    auto enqueueArrivals = [&](int t){
        while (nextIdx < n && procs[nextIdx].arrival <= t) {
            q.push(nextIdx);
            nextIdx++;
        }
    };

    std::cout << "== RR (q=" << quantum << ") ==\nPID\tArr\tBurst\tStart\tFinish\tWait\tTurn\n";
    enqueueArrivals(time);

    while (done < n) {
        if (q.empty()) {
            time = procs[nextIdx].arrival;
            enqueueArrivals(time);
            continue;
        }
        int i = q.front(); q.pop();

        if (!started[i]) { started[i] = 1; procs[i].start = time; }

        int ts = time;
        int slice = std::min(quantum, rt[i]);
        time += slice;
        rt[i] -= slice;
        gantt.push_back({procs[i].pid, ts, time});

        enqueueArrivals(time);

        if (rt[i] == 0) {
            procs[i].finish = time;
            procs[i].turnaround = procs[i].finish - procs[i].arrival;
            procs[i].waiting   = procs[i].turnaround - procs[i].burst;
            std::cout << procs[i].pid << '\t' << procs[i].arrival << '\t' << procs[i].burst << '\t'
                      << procs[i].start << '\t' << procs[i].finish << '\t'
                      << procs[i].waiting << '\t' << procs[i].turnaround << '\n';
            done++;
        } else {
            q.push(i);
        }
    }

    auto r = summarize(procs);
    std::cout << "AWT=" << r.awt << " ATT=" << r.att << "\n";
    std::cout << render_gantt_ascii(gantt) << "\n";
    return r;
}

// Priority non-preemptive
Result Scheduler::runPriorityNP(std::vector<Process> procs) {
    int n = (int)procs.size(), done = 0, time = 0;
    std::vector<int> used(n, 0);
    std::vector<GanttSlot> gantt;

    std::cout << "== Priority (non-preemptive) ==\nPID\tPrio\tArr\tBurst\tStart\tFinish\tWait\tTurn\n";
    while (done < n) {
        int idx = -1, bestPr = 1e9;
        for (int i = 0; i < n; ++i) {
            if (used[i]) continue;
            if (procs[i].arrival <= time && procs[i].priority < bestPr) {
                bestPr = procs[i].priority; idx = i;
            }
        }
        if (idx == -1) { time++; continue; }
        auto &p = procs[idx];
        p.start = time;
        p.finish = time + p.burst;
        p.waiting = p.start - p.arrival;
        p.turnaround = p.finish - p.arrival;
        time = p.finish;
        used[idx] = 1; done++;

        gantt.push_back({p.pid, p.start, p.finish});
        std::cout << p.pid << '\t' << p.priority << '\t' << p.arrival << '\t'
                  << p.burst << '\t' << p.start << '\t' << p.finish << '\t'
                  << p.waiting << '\t' << p.turnaround << '\n';
    }
    auto r = summarize(procs);
    std::cout << "AWT=" << r.awt << " ATT=" << r.att << "\n";
    std::cout << render_gantt_ascii(gantt) << "\n";
    return r;
}

// Priority preemptive
Result Scheduler::runPriorityP(std::vector<Process> procs) {
    int n = (int)procs.size();
    std::sort(procs.begin(), procs.end(),
              [](const Process& a, const Process& b){
                  if (a.arrival != b.arrival) return a.arrival < b.arrival;
                  if (a.priority != b.priority) return a.priority < b.priority;
                  return a.pid < b.pid;
              });
    std::vector<int> rt(n);
    for (int i = 0; i < n; ++i) rt[i] = procs[i].burst;

    int time = procs[0].arrival;
    int done = 0, cur = -1, curStart = time;
    std::vector<int> started(n, 0);
    std::vector<GanttSlot> gantt;

    auto pick = [&](int t)->int {
        int best = -1;
        int bestPr = 1e9, bestRt = 1e9;
        for (int i = 0; i < n; ++i) {
            if (procs[i].arrival <= t && rt[i] > 0) {
                if (procs[i].priority < bestPr ||
                   (procs[i].priority == bestPr && rt[i] < bestRt)) {
                    bestPr = procs[i].priority; bestRt = rt[i]; best = i;
                }
            }
        }
        return best;
    };

    std::cout << "== Priority (preemptive) ==\nPID\tPrio\tArr\tBurst\tStart\tFinish\tWait\tTurn\n";
    while (done < n) {
        int cand = pick(time);
        if (cand == -1) { time++; continue; }

        if (cand != cur) {
            if (cur != -1) gantt.push_back({procs[cur].pid, curStart, time});
            cur = cand; curStart = time;
            if (!started[cur]) { started[cur] = 1; procs[cur].start = time; }
        }

        rt[cur]--; time++;

        if (rt[cur] == 0) {
            gantt.push_back({procs[cur].pid, curStart, time});
            procs[cur].finish = time;
            procs[cur].turnaround = procs[cur].finish - procs[cur].arrival;
            procs[cur].waiting   = procs[cur].turnaround - procs[cur].burst;
            std::cout << procs[cur].pid << '\t' << procs[cur].priority << '\t'
                      << procs[cur].arrival << '\t' << procs[cur].burst << '\t'
                      << procs[cur].start << '\t' << procs[cur].finish << '\t'
                      << procs[cur].waiting << '\t' << procs[cur].turnaround << '\n';
            done++; cur = -1;
        }
    }

    auto r = summarize(procs);
    std::cout << "AWT=" << r.awt << " ATT=" << r.att << "\n";
    std::cout << render_gantt_ascii(gantt) << "\n";
    return r;
}
