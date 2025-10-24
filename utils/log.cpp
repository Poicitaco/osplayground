// utils/log.cpp
#include "log.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace utl {

std::string now_compact() {
    using namespace std::chrono;
    auto t = system_clock::to_time_t(system_clock::now());
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

bool ensure_dir(const std::string& dir) {
    namespace fs = std::filesystem;
    std::error_code ec;
    if (fs::exists(dir, ec)) return true;
    return fs::create_directories(dir, ec);
}

bool write_text(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f) return false;
    f << content;
    return true;
}

} // namespace utl
