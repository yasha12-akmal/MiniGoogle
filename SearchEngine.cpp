#include "SearchEngine.h"
#include "json.hpp"
#include <curl/curl.h>

using json = nlohmann::json;

// --- libcurl write callback (local helper) ---
static size_t _WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// --- Constructor ---
SearchEngine::SearchEngine()
    : m_apiKey("AIzaSyDZ3iqf1NZqJUOHuzG0xd9TJ938aWLIQ2g"),
      m_cx("a0d062dea4d71446b")
{
}

// --- Destructor ---
SearchEngine::~SearchEngine() {}

// --- EncodeQuery: replace spaces with %20 ---
std::string SearchEngine::EncodeQuery(const std::string& raw) {
    std::string encoded = raw;
    size_t pos = 0;
    while ((pos = encoded.find(' ', pos)) != std::string::npos) {
        encoded.replace(pos, 1, "%20");
        pos += 3;
    }
    return encoded;
}

// --- HttpGet: perform a simple GET request via libcurl ---
std::string SearchEngine::HttpGet(const std::string& url) {
    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

// --- Search: call Google Custom Search API and parse result ---
SearchResult SearchEngine::Search(const std::string& query) {
    SearchResult result;
    result.success = false;

    std::string encodedQuery = EncodeQuery(query);
    std::string url = "https://www.googleapis.com/customsearch/v1?key=" +
                      m_apiKey + "&cx=" + m_cx + "&q=" + encodedQuery;

    std::string response = HttpGet(url);

    try {
        auto j = json::parse(response);
        if (j.contains("items") && !j["items"].empty()) {
            result.success  = true;
            result.title    = j["items"][0].value("title", "");
            result.snippet  = j["items"][0].value("snippet", "");
            result.link     = j["items"][0].value("link", "");

            // Try to get preview image
            if (j["items"][0].contains("pagemap") &&
                j["items"][0]["pagemap"].contains("cse_image")) {
                result.imageUrl = j["items"][0]["pagemap"]["cse_image"][0].value("src", "");
            }
        }
        else {
            result.errorMessage = "No matches found.";
        }
    }
    catch (...) {
        result.errorMessage = "API Response Error.";
    }

    return result;
}
