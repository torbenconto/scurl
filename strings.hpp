//
// Created by tconto on 7/4/24.
//

#ifndef SPLIT_HPP
#define SPLIT_HPPP


#include <vector>
#include <string>

std::vector<std::string> split(const std::string& str, const std::string& delimiter);
bool starts_with(const std::string& str, const std::string& prefix);
bool ends_with(const std::string& str, const std::string& suffix);
void extract_host_and_path(const std::string& url, std::string& hostName, std::string& path);

#endif //SPLIT_HPP
