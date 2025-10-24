// sim/ipc/ipc.h
#pragma once
#include <queue>
#include <string>
#include <optional>
#include <iostream>

namespace ipc
{

    struct Pipe
    {
        std::queue<std::string> buffer;
        void write(const std::string &s) { buffer.push(s); }
        std::optional<std::string> read()
        {
            if (buffer.empty())
                return std::nullopt;
            auto s = buffer.front();
            buffer.pop();
            return s;
        }
    };

    enum Signal
    {
        SIG_INT = 2,
        SIG_KILL = 9,
        SIG_USR1 = 10
    };

    struct ProcessSim
    {
        int pid;
        bool alive = true;
        void handle(Signal sig);
    };

    void demo();

} // namespace ipc
