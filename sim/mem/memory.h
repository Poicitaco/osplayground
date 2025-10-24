// sim/mem/memory.h
#pragma once
#include <vector>
#include <optional>

namespace mem {

struct Paging {
    int pageSize = 1024;
    std::vector<int> pageTable; // page -> frame
    std::optional<int> translate(int page, int d) const; // frame*pageSize + d
};

struct Segment { int base; int limit; };

struct Segmentation {
    std::vector<Segment> segs;
    std::optional<int> translate(int s, int d) const; // base[s] + d (check)
};

void demo();

} // namespace mem
