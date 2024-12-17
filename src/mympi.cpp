#include "mympi.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <thread>
#include <chrono>


const int PORT = 5000;

MyMPI::MyMPI(int argc, char** argv) {
    if (argc < 3) {
        throw std::runtime_error("Usage: Proc <rank> <conf_filename>");
    }

    rank = std::stoi(argv[1]);
    const char* config_file = argv[2];

    FILE* file = fopen(config_file, "r");
    if (!file) throw std::runtime_error("Unable to open config file");

    int mode;
    fscanf(file, "%d", &mode);
    fscanf(file, "%d", &world_size);

    std::vector<std::pair<std::string, int>> addresses(world_size);
    for (int i = 0; i < world_size; ++i) {
        char ip[256];
        int port;
        fscanf(file, "%s %d", ip, &port);
        addresses[i] = {ip, port};
    }
    fclose(file);

    if (rank == 0) {
        setup_server(addresses[rank].first, PORT);
    } else {
        setup_client(addresses[0].first, PORT);
    }
}

void MyMPI::setup_server(const std::string& ip, int port) {
    std::cout << "Setting up server with IP: " << ip << " and PORT: " << port << std::endl;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) throw std::runtime_error("Failed to create socket");

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Failed to set socket options");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    listen(server_fd, world_size - 1);
    connections = new int[world_size - 1];

    for (int i = 1; i < world_size; ++i) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        connections[i - 1] = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        std::cout << "Connected to process " << i << " on port " << port << std::endl;
    }
}


void MyMPI::setup_client(const std::string& server_ip, int port) {
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) throw std::runtime_error("Failed to create socket");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    while (connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Process " << rank << " connected to server on port " << port << std::endl;
}

void MyMPI::send(int dest, const void* data, size_t size) {
    if (rank == 0) {
        ::send(connections[dest - 1], data, size, 0);
    } else {
        ::send(client_fd, data, size, 0);
    }
}

void MyMPI::receive(int source, void* buffer, size_t size) {
    if (rank == 0) {
        recv(connections[source - 1], buffer, size, 0);
    } else {
        recv(client_fd, buffer, size, 0);
    }
}

void MyMPI::barrier() {
    if (rank == 0) {
        char ready_signal = 'R';
        for (int i = 1; i < world_size; ++i) {
            recv(connections[i - 1], &ready_signal, 1, 0);
        }
        for (int i = 1; i < world_size; ++i) {
            ::send(connections[i - 1], &ready_signal, 1, 0);
        }
    } else {
        char ready_signal = 'R';
        ::send(client_fd, &ready_signal, 1, 0);
        recv(client_fd, &ready_signal, 1, 0);
    }
}

MyMPI::~MyMPI() {
    if (rank == 0) {
        close(server_fd);
        delete[] connections;
    } else {
        close(client_fd);
    }
}

int MyMPI::get_world_size() const { return world_size; }
int MyMPI::get_rank() const { return rank; }
