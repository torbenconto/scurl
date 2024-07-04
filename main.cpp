#include <iostream>
#include <sys/socket.h>
#include "strings.hpp"
#include <netdb.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
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

    std::vector<std::string> parts = split(urlStr, "/");
    if (parts.size() == 1) {
        url = parts[0].c_str();
    } else {
        url = parts[0].c_str();
        parts.erase(parts.begin());
        // Set the path
        std::string path = "/";
        for (const std::string& part : parts) {
            path += part;
            if (!ends_with(path, "/")) {
                path += "/";
            }
        }
        std::cout << "Path: " << path << std::endl;
    }

    int port = 80; // Default port

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

    if (getaddrinfo(url, nullptr, &hints, &res) != 0) {
        std::cerr << "Failed to resolve hostname\n";
        return 1;
    }


    // Returns status code
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating sock\n";
        return 1;
    }

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
    std::string request = "GET / HTTP/1.1\r\nHost: " + std::string(url) + "\r\nConnection: close\r\n\r\n";
    if (send(sock, request.c_str(), request.size(), 0) < 0) {
        std::cerr << "Failed to send request\n";
        return 1;
    }

    // Recv response
    char buffer[4096] = {};
    std::string response;

    for (;;) {
        ssize_t bytesRecv = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesRecv <= 0) {
            break;
        }
        response.append(buffer, bytesRecv);
    }

    std::cout << response << std::endl;

    close(sock);

    return 0;
}
