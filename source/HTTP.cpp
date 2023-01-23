#include <curl/curl.h>
#include <fmt/format.h>
#include "HTTP.hpp"
#include <fstream>

size_t writeToMemFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

size_t writeToFileFunction(void* ptr, size_t size, size_t nmemb, std::ofstream* stream) {
    stream->write((char*)ptr, size*nmemb);
    return size * nmemb;
}

int progress_callback(void *clientp,
                      curl_off_t dltotal,
                      curl_off_t dlnow,
                      curl_off_t ultotal,
                      curl_off_t ulnow){
    auto callback = static_cast<FunctionCallback*>(clientp);
    callback->fn(dltotal,dlnow,ultotal,ulnow);
    return CURL_PROGRESSFUNC_CONTINUE;
}

fetchResult fetch_to_mem(const std::string& url) {
	auto curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.5 Safari/605.1.15");
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);		// probably shouldn't do this...

		std::string response_string;
		std::string header_string;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToMemFunction);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

		auto result = curl_easy_perform(curl);
		long response_code = 0;
		if (result != CURLE_OK) {
			throw std::runtime_error(fmt::format("Curl failed: {}", curl_easy_strerror(result)));
		}
		else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		}
		curl_easy_cleanup(curl);
		curl = NULL;
		return { response_string, response_code };
	}
	else {
		throw std::runtime_error("curl init failed");
		return { "" , -1};
	}
}

long stream_to_file(const std::string& url, const std::filesystem::path& output, const FunctionCallback& progressCallback){
    auto curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // must be set for progress callback to be invoked
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progressCallback);    // clientp for progresscallback
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.5 Safari/605.1.15");
        
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);        // probably shouldn't do this...

        std::ofstream outstream(output, std::ios::binary);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToFileFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outstream);
        
        auto result = curl_easy_perform(curl);
        long response_code = 0;
        if (result != CURLE_OK) {
            throw std::runtime_error(fmt::format("Curl failed: {}", curl_easy_strerror(result)));
        }
        else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        }
        curl_easy_cleanup(curl);
        curl = NULL;
        
        return response_code;
    }
    else {
        throw std::runtime_error("curl init failed");
        return -1;
    }
}
