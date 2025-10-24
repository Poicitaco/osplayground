// sim/ipc/ipc.cpp
#include "ipc.h"
#include <iostream>

namespace ipc
{

    void ProcessSim::handle(Signal sig)
    {
        if (sig == SIG_INT)
            std::cout << "PID " << pid << " caught SIGINT\n";
        if (sig == SIG_USR1)
            std::cout << "PID " << pid << " received SIGUSR1 (custom)\n";
        if (sig == SIG_KILL)
        {
            std::cout << "PID " << pid << " killed\n";
            alive = false;
        }
    }

    void demo()
    {
        std::cout << "== IPC/Signal Demo ==\n";
        Pipe p;
        ProcessSim parent{100}, child{101};
        std::cout << "Parent PID: " << parent.pid << "\n";

        p.write("Hello from Parent!");
        auto msg = p.read();
        if (msg)
            std::cout << "Child received: " << *msg << "\n";
        child.handle(SIG_INT);
        child.handle(SIG_USR1);
        child.handle(SIG_KILL);
    }

} // namespace ipc

namespace ipc_demo
{
    void run() { ipc::demo(); }
}
