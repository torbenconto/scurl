//
// Created by tconto on 7/4/24.
//

#ifndef HTTP_HPP
#define HTTP_HPP

#include <netdb.h>
#include <string>

class http {
public:
    static std::tuple<addrinfo*, int> resolve_hostname(const std::string& hostName);
    static int create_socket();
    static int send_request(int sock, int port, const addrinfo *res, const std::string& hostName, const std::string& path);
    static int read_response(int sock);
};


#endif //HTTP_HPP
