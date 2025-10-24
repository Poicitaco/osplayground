// utils/log.h
#pragma once
#include <string>

namespace utl
{
    std::string now_compact();               // YYYYMMDD_HHMMSS
    bool ensure_dir(const std::string &dir); // tạo thư mục nếu chưa có
    bool write_text(const std::string &path, const std::string &content);
}
