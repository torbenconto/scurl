//
// Created by tconto on 7/4/24.
//

#include "http.hpp"

#include <iostream>
#include <sys/socket.h>
#include "strings.hpp"
#include <netdb.h>
#include <unistd.h>

int http::create_socket()
{
    // Returns status code
    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    return sock;
}

int http::send_request(const int sock, const int port, const addrinfo *res, const std::string &hostName, const std::string &path)
{
    sockaddr_in serverAddress{};
    // IPv4
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;

    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to connect to the server\n";
        return 1;
    }

    // Build an HTTP request
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + std::string(hostName) + "\r\nConnection: close\r\n\r\n";
    if (send(sock, request.c_str(), request.size(), 0) < 0) {
        return 1;
    }

    return 0;
}

int http::read_response(const int sock)
{
    // Recv response
    char buffer[4096] = {};
    std::string response;

    for (;;) {
        const ssize_t bytesRecv = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesRecv <= 0) {
            break;
        }
        response.append(buffer, bytesRecv);
    }

    std::cout << response << std::endl;

    return sock;
}


std::tuple<addrinfo*, int> http::resolve_hostname(const std::string& hostName)
{
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (getaddrinfo(hostName.c_str(), nullptr, &hints, &res) != 0) {
        return std::tuple{res, 1};
    }

    return std::tuple{res, 0};
}