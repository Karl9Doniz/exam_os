#include "mympi.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

MyMPI::MyMPI(int argc, char** argv) {
    if (argc < 3) {
        throw std::runtime_error("Usage: program <rank> <config_file>");
    }
    
    rank_ = std::stoi(argv[1]);
    initialize(argv[2]);
}

MyMPI::~MyMPI() {
    for (int sock : connections_) {
        if (sock > 0) {
            close(sock);
        }
    }
}

void MyMPI::initialize(const std::string& config_file) {
    std::ifstream config(config_file);
    if (!config.is_open()) {
        throw std::runtime_error("Cannot open config file");
    }

    config >> size_;
    connections_.resize(size_, -1);
    
    setupConnections();
}

void MyMPI::setupConnections() {
    // Create server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // Allow port reuse
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind server socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8000 + rank_);

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    listen(server_sock, size_);

    // Connect to processes with higher rank
    for (int i = rank_ + 1; i < size_; i++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in peer_addr;
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_addr.s_addr = INADDR_ANY;
        peer_addr.sin_port = htons(8000 + i);

        // Try to connect with retries
        int retries = 0;
        while (retries < 5) {
            if (connect(sock, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) == 0) {
                connections_[i] = sock;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            retries++;
        }
        if (retries == 5) {
            throw std::runtime_error("Failed to connect to peer");
        }
    }

    // Accept connections from processes with lower rank
    for (int i = 0; i < rank_; i++) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            throw std::runtime_error("Failed to accept connection");
        }
        connections_[i] = client_sock;
    }

    close(server_sock);
}
