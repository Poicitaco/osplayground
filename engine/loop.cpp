// engine/loop.cpp
#include "loop.h"
#include <iostream>

namespace cpu_demo { void run(); }
namespace mem_demo { void run(); }
namespace ipc_demo { void run(); }

void Engine::showMenu() {
    std::cout << "=== OSPlayground ===\n";
    std::cout << "1) CPU Scheduling demo (FCFS/SJF/RR/Priority)\n";
    std::cout << "2) Memory demo (Paging/Segmentation)\n";
    std::cout << "3) IPC/Signal demo (Pipe/Signal)\n";
    std::cout << "0) Exit\n";
    std::cout << "Your choice: ";
}

void Engine::handleInput(int choice) {
    if (choice == 1) demoCPU();
    else if (choice == 2) demoMemory();
    else if (choice == 3) demoIPC();
}

void Engine::run() {
    std::cout << "[Engine] Menu ready\n";
    while (true) {
        showMenu();
        int c = -1;
        if (!(std::cin >> c)) return;
        if (c == 0) return;
        handleInput(c);
        std::cout << "\n";
    }
}

void Engine::demoCPU()    { cpu_demo::run(); }
void Engine::demoMemory() { mem_demo::run(); }
void Engine::demoIPC()    { ipc_demo::run(); }
