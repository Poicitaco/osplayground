// sim/mem/replacement.cpp
#include "replacement.h"
#include <deque>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>

namespace mem
{

    static void snapshot(std::vector<std::vector<int>> &states,
                         const std::vector<int> &frames)
    {
        states.push_back(frames);
    }

    ReplResult simulate_fifo(const std::vector<int> &ref, int F)
    {
        ReplResult R;
        R.frames = F;
        R.faults = 0;
        std::vector<int> frames(F, -1);
        std::deque<int> q;

        for (int p : ref)
        {
            bool hit = false;
            for (int f : frames)
                if (f == p)
                {
                    hit = true;
                    break;
                }
            if (!hit)
            {
                R.faults++;
                if ((int)q.size() < F)
                {
                    int idx = (int)q.size();
                    frames[idx] = p;
                    q.push_back(idx);
                }
                else
                {
                    int evict_idx = q.front();
                    q.pop_front();
                    frames[evict_idx] = p;
                    q.push_back(evict_idx);
                }
            }
            snapshot(R.frame_states, frames);
        }
        return R;
    }

    ReplResult simulate_lru(const std::vector<int> &ref, int F)
    {
        ReplResult R;
        R.frames = F;
        R.faults = 0;
        std::vector<int> frames(F, -1), last_used(F, -1);
        int t = 0;

        auto find_in_frames = [&](int page) -> int
        {
            for (int i = 0; i < F; ++i)
                if (frames[i] == page)
                    return i;
            return -1;
        };
        auto free_slot = [&]() -> int
        {
            for (int i = 0; i < F; ++i)
                if (frames[i] == -1)
                    return i;
            return -1;
        };
        auto lru_index = [&]() -> int
        {
            int idx = 0;
            for (int i = 1; i < F; ++i)
                if (last_used[i] < last_used[idx])
                    idx = i;
            return idx;
        };

        for (int page : ref)
        {
            int pos = find_in_frames(page);
            if (pos != -1)
            {
                last_used[pos] = t;
            }
            else
            {
                R.faults++;
                int slot = free_slot();
                if (slot == -1)
                    slot = lru_index();
                frames[slot] = page;
                last_used[slot] = t;
            }
            snapshot(R.frame_states, frames);
            t++;
        }
        return R;
    }

    // Optimal: thay trang có lần dùng kế tiếp xa nhất (hoặc không dùng nữa)
    ReplResult simulate_opt(const std::vector<int> &ref, int F)
    {
        ReplResult R;
        R.frames = F;
        R.faults = 0;
        std::vector<int> frames(F, -1);

        auto find_in_frames = [&](int page) -> int
        {
            for (int i = 0; i < F; ++i)
                if (frames[i] == page)
                    return i;
            return -1;
        };
        auto free_slot = [&]() -> int
        {
            for (int i = 0; i < F; ++i)
                if (frames[i] == -1)
                    return i;
            return -1;
        };
        auto next_use_index = [&](int page, int start) -> int
        {
            for (int k = start; k < (int)ref.size(); ++k)
                if (ref[k] == page)
                    return k;
            return std::numeric_limits<int>::max(); // không dùng nữa
        };

        for (int t = 0; t < (int)ref.size(); ++t)
        {
            int page = ref[t];
            int pos = find_in_frames(page);
            if (pos == -1)
            {
                R.faults++;
                int slot = free_slot();
                if (slot == -1)
                {
                    int victim = 0, far = -1;
                    for (int i = 0; i < F; ++i)
                    {
                        int nu = next_use_index(frames[i], t + 1);
                        if (nu > far)
                        {
                            far = nu;
                            victim = i;
                        }
                    }
                    slot = victim;
                }
                frames[slot] = page;
            }
            snapshot(R.frame_states, frames);
        }
        return R;
    }

    std::string render_frames_ascii(const ReplResult &r, int empty_mark)
    {
        if (r.frame_states.empty())
            return "[no steps]";
        int steps = (int)r.frame_states.size();
        int F = r.frames;

        std::ostringstream out;
        out << "Frames=" << F << " | Faults=" << r.faults << "\n";
        out << "Step :";
        for (int s = 0; s < steps; ++s)
            out << std::setw(3) << s;
        out << "\n";
        for (int i = 0; i < F; ++i)
        {
            out << "F" << i << "   :";
            for (int s = 0; s < steps; ++s)
            {
                int val = r.frame_states[s][i];
                if (val == empty_mark)
                    out << std::setw(3) << '.';
                else
                    out << std::setw(3) << val;
            }
            out << "\n";
        }
        return out.str();
    }

} // namespace mem
