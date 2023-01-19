#pragma once
#include <string>
#include <filesystem>

struct fetchResult {
    std::string text;
    long code;
};
fetchResult fetch_to_mem(const std::string& url);

long stream_to_file(const std::string& url, const std::filesystem::path& output);
