//
// Created by tconto on 7/4/24.
//

#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <map>
#include <vector>


class http {


public:
    struct response {
        int status;
        std::map<std::string, std::string> headers;

        std::string body;
    };
    static std::pair<int, response> make_ssl_request(const std::string& hostName, const std::string& path, int port);
    static std::pair<int, response> make_request(const std::string& hostName, const std::string& path, int port);
    static response parse_response(const std::string& response);
    static std::string parse_chunked_body(const std::vector<std::string>& lines, size_t start_index);
};


#endif //HTTP_HPP
