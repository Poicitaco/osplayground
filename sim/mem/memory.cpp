// sim/mem/memory.cpp
#include "memory.h"
#include "replacement.h"
#include "../../utils/log.h"
#include <iostream>
#include <sstream>
#include <string>

namespace mem
{

    std::optional<int> Paging::translate(int page, int d) const
    {
        if (page < 0 || page >= (int)pageTable.size())
            return std::nullopt;
        if (d < 0 || d >= pageSize)
            return std::nullopt;
        int frame = pageTable[page];
        return frame * pageSize + d;
    }

    std::optional<int> Segmentation::translate(int s, int d) const
    {
        if (s < 0 || s >= (int)segs.size())
            return std::nullopt;
        if (d < 0 || d >= segs[s].limit)
            return std::nullopt;
        return segs[s].base + d;
    }

    static std::vector<int> parse_ints_line(const std::string &line)
    {
        std::vector<int> v;
        std::istringstream iss(line);
        int x;
        while (iss >> x)
            v.push_back(x);
        return v;
    }

    void demo()
    {
        std::cout << "== Memory Demo ==\n";

        // Demo translate
        Paging pg;
        pg.pageSize = 1024;
        pg.pageTable = {2, 5, 7};
        auto pa1 = pg.translate(1, 100);
        std::cout << "Paging: page=1, d=100 -> " << (pa1 ? std::to_string(*pa1) : "Invalid") << "\n";

        Segmentation seg;
        seg.segs = {{1000, 200}, {3000, 500}};
        auto pa2 = seg.translate(0, 150);
        std::cout << "Segmentation: s=0, d=150 -> " << (pa2 ? std::to_string(*pa2) : "Invalid") << "\n";

        // Replacement input
        std::cin.ignore(1 << 20, '\n');
        std::cout << "\nNhập ref string (vd: 7 0 1 2 0 3 0 4 2 3 0 3). Bỏ trống = mặc định:\n> ";
        std::string line;
        std::getline(std::cin, line);
        std::vector<int> ref = line.empty() ? std::vector<int>{7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3} : parse_ints_line(line);
        if (ref.empty())
            ref = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3};

        std::cout << "Nhập số frames (mặc định 3): ";
        int frames = 3;
        if (!(std::cin >> frames) || frames <= 0)
            frames = 3;

        auto fifo = simulate_fifo(ref, frames);
        auto lru = simulate_lru(ref, frames);
        auto opt = simulate_opt(ref, frames);

        std::cout << "\n-- FIFO --\n"
                  << render_frames_ascii(fifo);
        std::cout << "\n-- LRU  --\n"
                  << render_frames_ascii(lru);
        std::cout << "\n-- OPT  --\n"
                  << render_frames_ascii(opt);

        // Log
        utl::ensure_dir("assets/logs");
        std::ostringstream log;
        log << "[Memory Run] " << utl::now_compact() << "\n"
            << "Ref: ";
        for (auto x : ref)
            log << x << ' ';
        log << "\n"
            << "Frames: " << frames << "\n\n"
            << "[FIFO]\n"
            << render_frames_ascii(fifo)
            << "\n[LRU]\n"
            << render_frames_ascii(lru)
            << "\n[OPT]\n"
            << render_frames_ascii(opt);
        std::string fn = "assets/logs/mem_" + utl::now_compact() + ".txt";
        if (utl::write_text(fn, log.str()))
            std::cout << "\n(Lưu log) " << fn << "\n";
        else
            std::cout << "\n[!] Không ghi được log.\n";
    }

} // namespace mem

namespace mem_demo
{
    void run() { mem::demo(); }
}
