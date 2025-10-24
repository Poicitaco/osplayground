// engine/loop.h
#pragma once
class Engine {
public:
    void run();
private:
    void showMenu();
    void handleInput(int choice);
    void demoCPU();
    void demoMemory();
    void demoIPC();
};
