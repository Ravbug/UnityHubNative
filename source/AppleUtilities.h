#pragma once
#include <cstdint>

void getCFBundleVersionFromPlist(const char* path, char* outbuf, uint8_t outbuf_size);

enum architecture{
    Unknown = 0,
    x86_64 = 1 << 1,
    arm64 = 1 << 2,
};
int getArchitectureFromBundle(const char* path);
