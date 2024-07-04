#include <iostream>
#include <sys/socket.h>
#include "strings.hpp"
#include "http.hpp"
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

    int port = 80; // Default port

    if (argc >= 3) {
        const char* portArg = argv[2];
        port = std::atoi(portArg);
        if (port <= 0) {
            std::cerr << "Invalid port number\n";
            return 1;
        }
    }

    std::tuple<addrinfo*, int> addrTuple = http::resolve_hostname(hostName);
    if (std::get<1>(addrTuple) != 0) {
        std::cerr << "Failed to resolve hostname\n";
        return 1;
    }

    int sock = http::create_socket();
    if (sock < 0) {
        std::cerr << "Error creating sock\n";
        return 1;
    }

    if (http::send_request(sock, port, std::get<0>(addrTuple), hostName, path) != 0) {
        std::cerr << "Failed to send request\n";
        return 1;
    }

    if (http::read_response(sock) < 0) {
        std::cerr << "Failed to read response\n";
        return 1;
    }

    close(sock);

    return 0;
}
