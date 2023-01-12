#include "AppleUtilities.h"
#import <Cocoa/Cocoa.h>

void getCFBundleVersionFromPlist(const char* path, char* outbuf, uint8_t outbuf_size){
    NSString* pstr = [NSString stringWithUTF8String:path];
    NSDictionary* dict = [NSDictionary dictionaryWithContentsOfFile:pstr];
    NSString* item = dict[@"CFBundleVersion"];
    
    memset(outbuf, 0, outbuf_size);
    strncpy(outbuf, [item cStringUsingEncoding:NSUTF8StringEncoding], outbuf_size);
}

int getArchitectureFromBundle(const char* path){
    auto bundle = [NSBundle bundleWithPath:[NSString stringWithUTF8String:path]];
    auto archs = [bundle executableArchitectures];
    
    int archmask = architecture::Unknown;
    
    for(NSNumber* archptr in archs){
        auto arch = [archptr intValue];
        if (arch == NSBundleExecutableArchitectureX86_64){
            archmask |= architecture::x86_64;
        }
        if (@available(macOS 11.0, *)) {
            if (arch == NSBundleExecutableArchitectureARM64){
                archmask |= architecture::arm64;
            }
        } 
    }
    return archmask;
}
