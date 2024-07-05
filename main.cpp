#include <iostream>
#include "http.hpp"
#include "strings.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <url> [port]\n";
        return 1;
    }

    const char* url = argv[1];
    if (starts_with(url, "http://") || starts_with(url, "https://")) {
        std::cerr << "Please do not include a protocol\n";
        return 1;
    }

    std::string urlStr = url;
    std::string path;
    std::string hostName;
    extract_host_and_path(urlStr, hostName, path);

    int port = 80;
    if (argc >= 3) {
        const char* portArg = argv[2];
        port = std::atoi(portArg);
        if (port <= 0) {
            std::cerr << "Invalid port number\n";
            return 1;
        }
    }

    int status = 0;
    http::response response;
    do {
        if (port == 443) {
            auto result = http::make_ssl_request(hostName, path, port);
            status = result.first;
            response = result.second;
        } else {
            auto result = http::make_request(hostName, path, port);
            status = result.first;
            response = result.second;
        }

        if (status != 0) {
            std::cerr << "Failed to make request\n";
            return 1;
        }

        // If status code is a 3xx, follow the redirect
        if (response.status >= 300 && response.status < 400 && response.status != 304) {
            std::string location = response.headers["Location"];
            if (location.empty()) {
                std::cerr << "Redirect status code without a location header\n";
                return 1;
            }
            if (starts_with(location, "http://")) {
                location = location.substr(7);
                extract_host_and_path(location, hostName, path);
                auto result = http::make_request(hostName, path, port);
                status = result.first;
                response = result.second;
            } else if (starts_with(location, "https://")) {
                port = 443;
                location = location.substr(8);
                extract_host_and_path(location, hostName, path);
                auto result = http::make_ssl_request(hostName, path, port);
                status = result.first;
                response = result.second;
            }
        } else {
            std::cout << response.body << std::endl;
            break;
        }
    } while (status == 0);

    return 0;
}
