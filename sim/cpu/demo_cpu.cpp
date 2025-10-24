// sim/cpu/demo_cpu.cpp
#include "scheduler.h"
#include "../../utils/log.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::vector<Process> default_set() {
    return {{1,0,10,2},{2,0,5,1},{3,0,8,3}};
}

static std::vector<Process> read_processes_from_cli() {
    std::vector<Process> ps;
    std::cout << "Nhập số tiến trình: ";
    int n; if (!(std::cin >> n) || n <= 0) { std::cin.clear(); std::cin.ignore(1<<20,'\n'); return default_set(); }
    std::cout << "Nhập: pid arrival burst priority (mỗi dòng 4 số)\n";
    for (int i=0;i<n;++i){
        Process p{};
        if(!(std::cin >> p.pid >> p.arrival >> p.burst >> p.priority)){
            std::cin.clear(); std::cin.ignore(1<<20,'\n');
            std::cout << "Dữ liệu không hợp lệ. Dùng dataset mặc định.\n";
            return default_set();
        }
        ps.push_back(p);
    }
    return ps;
}

static std::string dataset_to_str(const std::vector<Process>& ps) {
    std::ostringstream oss;
    for (auto &p: ps) oss << "(P" << p.pid << ":" << p.arrival << "," << p.burst << ",prio=" << p.priority << ") ";
    return oss.str();
}

namespace cpu_demo {
void run() {
    Scheduler sch;
    std::cout << "[CPU DEMO]\n1) Dataset mặc định (P1=10,P2=5,P3=8)\n2) Tự nhập dataset\nChọn: ";
    int opt=1; std::cin >> opt;
    auto ps = (opt==2) ? read_processes_from_cli() : default_set();

    std::cout << "\nChọn thuật toán:\n"
              << " 1) FCFS\n 2) SJF (non-preemptive)\n 3) SRTF (preemptive SJF)\n"
              << " 4) RR (preemptive)\n 5) Priority (non-preemptive)\n 6) Priority (preemptive)\n"
              << " 0) Chạy tất cả\nChọn: ";
    int alg=0; std::cin >> alg;

    auto do_log = [&](const std::string& algo, const std::string& extra, const Result& r){
        utl::ensure_dir("assets/logs");
        std::ostringstream L;
        L << "[CPU Run] " << utl::now_compact() << "\n"
          << "Algo: " << algo << " " << extra << "\n"
          << "Dataset: " << dataset_to_str(ps) << "\n"
          << "AWT=" << r.awt << " ATT=" << r.att << "\n";
        std::string fn = "assets/logs/cpu_" + algo + "_" + utl::now_compact() + ".txt";
        if (utl::write_text(fn, L.str()))
            std::cout << "(Lưu log) " << fn << "\n";
        else
            std::cout << "[!] Không ghi được log.\n";
    };

    auto run_all = [&](){
        std::cout << "[Dataset] " << dataset_to_str(ps) << "\n";
        auto r1 = sch.runFCFS(ps);          do_log("fcfs","",r1);
        auto r2 = sch.runSJF(ps);           do_log("sjf","",r2);
        auto r3 = sch.runSRTF(ps);          do_log("srtf","",r3);
        auto r4 = sch.runRR(ps, 4);         do_log("rr","q=4",r4);
        auto r5 = sch.runPriorityNP(ps);    do_log("prio_np","",r5);
        auto r6 = sch.runPriorityP(ps);     do_log("prio_p","",r6);
    };

    switch(alg){
        case 1: { auto r = sch.runFCFS(ps);        do_log("fcfs","",r); break; }
        case 2: { auto r = sch.runSJF(ps);         do_log("sjf","",r); break; }
        case 3: { auto r = sch.runSRTF(ps);        do_log("srtf","",r); break; }
        case 4: {
            std::cout << "Nhập quantum (mặc định 4): ";
            int q=4; std::cin >> q; if(q<=0) q=4;
            auto r = sch.runRR(ps, q);             do_log("rr","q="+std::to_string(q),r);
            break;
        }
        case 5: { auto r = sch.runPriorityNP(ps);  do_log("prio_np","",r); break; }
        case 6: { auto r = sch.runPriorityP(ps);   do_log("prio_p","",r); break; }
        default: run_all(); break;
    }
}
}
