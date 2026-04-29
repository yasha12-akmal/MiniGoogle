#pragma once
#include <string>

// --- Struct to hold a single search result ---
struct SearchResult {
    bool        success;
    std::string title;
    std::string snippet;
    std::string link;
    std::string imageUrl;
    std::string errorMessage;
};

// --- SearchEngine class ---
// Wraps Google Custom Search API calls
class SearchEngine {
public:
    SearchEngine();
    ~SearchEngine();

    // Perform a search query; returns a populated SearchResult
    SearchResult Search(const std::string& query);

private:
    std::string m_apiKey;
    std::string m_cx;

    // URL-encode spaces in query string
    std::string EncodeQuery(const std::string& raw);

    // Internal HTTP GET using libcurl
    std::string HttpGet(const std::string& url);
};
