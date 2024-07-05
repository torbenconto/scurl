//
// Created by tconto on 7/4/24.
//

#include "strings.hpp"


std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

bool starts_with(const std::string& str, const std::string& prefix) {
    return str.rfind(prefix, 0) == 0;
}

bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// No protocol should be included
void extract_host_and_path(const std::string& url, std::string& hostName, std::string& path) {
    std::vector<std::string> parts = split(url, "/");
    hostName = parts.at(0);

    if (parts.size() != 1) {
        parts.erase(parts.begin());
        // Set the path
        path = "/";
        for (const std::string& part : parts) {
            path += part;
            if (!ends_with(path, "/")) {
                path += "/";
            }
        }
    }
}