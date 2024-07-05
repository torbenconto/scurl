#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include "strings.hpp"
#include <netdb.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    SSL_library_init();
    SSL_load_error_strings();
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <url> [port]\n";
        return 1;
    }

    const char* url = argv[1];
    if (starts_with(url, "http://") || starts_with(url, "https://")) {
        std::cerr << "Please do not include a protocol\n";
        return 1;
    }
    const std::string urlStr = url;

    std::string path;
    std::vector<std::string> parts = split(urlStr, "/");
    std::string hostName = parts.at(0);

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
        std::cout << "Path: " << path << std::endl;
    }

    int port = 443; // Default HTTPS port

    if (argc >= 3) {
        const char* portArg = argv[2];
        port = std::atoi(portArg);
        if (port <= 0) {
            std::cerr << "Invalid port number\n";
            return 1;
        }
    }

    addrinfo hints{}, *res;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    std::cout << hostName << std::endl;

    if (getaddrinfo(hostName.c_str(), nullptr, &hints, &res) != 0) {
        std::cerr << "Failed to resolve hostname\n";
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating sock\n";
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;

    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to connect to the server\n";
        return 1;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    SSL_set_tlsext_host_name(ssl, hostName.c_str());
    SSL_set1_host(ssl, hostName.c_str());

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL connection failed\n";
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // Build an HTTPS request
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + std::string(hostName) + "\r\nConnection: close\r\n\r\n";
    if (SSL_write(ssl, request.c_str(), request.size()) <= 0) {
        std::cerr << "Failed to send request\n";
        return 1;
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

    std::cout << response << std::endl;

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    return 0;
}
