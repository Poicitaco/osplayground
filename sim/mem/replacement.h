// sim/mem/replacement.h
#pragma once
#include <vector>
#include <string>

namespace mem
{

    struct ReplResult
    {
        int frames = 0;
        int faults = 0;
        std::vector<std::vector<int>> frame_states; // snapshot sau mỗi step
    };

    ReplResult simulate_fifo(const std::vector<int> &refString, int frames);
    ReplResult simulate_lru(const std::vector<int> &refString, int frames);
    ReplResult simulate_opt(const std::vector<int> &refString, int frames);

    std::string render_frames_ascii(const ReplResult &r, int empty_mark = -1);

} // namespace mem
