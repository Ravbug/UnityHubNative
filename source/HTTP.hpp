#pragma once
#include <string>

struct fetchResult {
    std::string text;
    long code;
};
fetchResult fetch(const std::string& url);
