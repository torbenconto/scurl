//
// Created by tconto on 7/4/24.
//

#include "http.hpp"

#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include "strings.hpp"
#include <netdb.h>
#include <unistd.h>

// Marshal the response into an response object
http::response http::parse_response(const std::string& response) {
    http::response res;
    std::vector<std::string> lines = split(response, "\r\n");

    // Parse the status line
    std::vector<std::string> statusLine = split(lines.at(0), " ");
    res.status = std::stoi(statusLine.at(1));

    // Parse the headers
    for (size_t i = 1; i < lines.size(); i++) {
        std::string line = lines.at(i);
        if (line.empty()) {
            // The body starts after the headers
            res.body = lines.at(i + 1);
            break;
        }

        std::vector<std::string> parts = split(line, ": ");
        res.headers[parts.at(0)] = parts.at(1);
    }

    return res;
}

std::pair<int, http::response> http::make_ssl_request(const std::string& hostName, const std::string& path, int port) {
    SSL_library_init();
    SSL_load_error_strings();
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());

    addrinfo hints{}, *res;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (getaddrinfo(hostName.c_str(), nullptr, &hints, &res) != 0) {
        std::cerr << "Failed to resolve hostname\n";
        return {1, response{}};
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating sock\n";
        return {1, response{}};
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;

    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to connect to the server\n";
        return {1, response{}};
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    SSL_set_tlsext_host_name(ssl, hostName.c_str());
    SSL_set1_host(ssl, hostName.c_str());

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL connection failed\n";
        ERR_print_errors_fp(stderr);
        return {1, response{}};
    }

    // Build an HTTPS request
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + std::string(hostName) + "\r\nConnection: close\r\n\r\n";
    if (SSL_write(ssl, request.c_str(), request.size()) <= 0) {
        std::cerr << "Failed to send request\n";
        return {1, response{}};
    }

    // Receive response
    char buffer[4096] = {};
    std::string response;
    int bytesRecv;

    do {
        bytesRecv = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytesRecv > 0) {
            response.append(buffer, bytesRecv);
        }
    } while (bytesRecv > 0);

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    http::response parsedResponse = parse_response(response);


    return {0, parsedResponse};
}

std::pair<int, http::response>  http::make_request(const std::string& hostName, const std::string& path, int port) {
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (getaddrinfo(hostName.c_str(), nullptr, &hints, &res) != 0) {
        std::cerr << "Failed to resolve hostname\n";
        return {1, response{}};
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating sock\n";
        return {1, response{}};
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;

    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to connect to the server\n";
        return {1, response{}};
    }

    // Build an HTTP request
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + std::string(hostName) + "\r\nConnection: close\r\n\r\n";

    // Connect
    if (send(sock, request.c_str(), request.size(), 0) < 0) {
        std::cerr << "Failed to send request\n";
        return {1, response{}};
    }

    // Receive response
    char buffer[4096] = {};
    std::string response;
    for (;;) {
        ssize_t bytesRecv = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesRecv <= 0) {
            break;
        }
        response.append(buffer, bytesRecv);
    }

    http::response parsedResponse = parse_response(response);

    return {0, parsedResponse};
}
