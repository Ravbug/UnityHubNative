#include "AppleUtilities.h"
#import <Cocoa/Cocoa.h>

void getCFBundleVersionFromPlist(const char* path, char* outbuf, uint8_t outbuf_size){
    NSString* pstr = [NSString stringWithUTF8String:path];
    NSDictionary* dict = [NSDictionary dictionaryWithContentsOfFile:pstr];
    NSString* item = dict[@"CFBundleVersion"];
    
    memset(outbuf, 0, outbuf_size);
    strncpy(outbuf, [item cStringUsingEncoding:NSUTF8StringEncoding], outbuf_size);
}
