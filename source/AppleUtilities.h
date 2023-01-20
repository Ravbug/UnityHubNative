#pragma once
#include <cstdint>
#include <string>
#include <filesystem>

void getCFBundleVersionFromPlist(const char* path, char* outbuf, uint8_t outbuf_size);

enum architecture{
    Unknown = 0,
    x86_64 = 1 << 1,
    arm64 = 1 << 2,
};
int getArchitectureFromBundle(const char* path);

int executeProcess(const std::string& cmd, const std::vector<std::string>& arguments, const std::filesystem::path& pwd);
