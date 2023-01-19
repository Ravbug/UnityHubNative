#pragma once
#include <string>
#include <filesystem>
#include <functional>

struct fetchResult {
    std::string text;
    long code;
};
fetchResult fetch_to_mem(const std::string& url);

struct FunctionCallback{
    // download total, current download, upload total, current upload
    std::function<void(long,long,long,long)> fn;
    
    template<typename T>
    FunctionCallback(const T& fn) : fn(fn){}
};

long stream_to_file(const std::string& url, const std::filesystem::path& output, const FunctionCallback& progressCallback);
