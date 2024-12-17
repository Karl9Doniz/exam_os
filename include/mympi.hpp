#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class MyMPI {
public:
    MyMPI(int argc, char** argv);
    ~MyMPI();

    MyMPI(const MyMPI&) = delete;
    MyMPI& operator=(const MyMPI&) = delete;

    int getRank() const { return rank_; }
    int getSize() const { return size_; }

    template<typename T>
    void send(int destination, const T& data) {
        if (destination >= size_) {
            throw std::runtime_error("Invalid destination rank");
        }
        
        // Use :: to explicitly call the global send function
        ssize_t result = ::send(connections_[destination], &data, sizeof(T), 0);
        if (result < 0) {
            throw std::runtime_error("Failed to send data");
        }
    }

    template<typename T>
    T receive(int source) {
        if (source >= size_) {
            throw std::runtime_error("Invalid source rank");
        }

        T data;
        ssize_t result = ::recv(connections_[source], &data, sizeof(T), 0);
        if (result < 0) {
            throw std::runtime_error("Failed to receive data");
        }
        return data;
    }

private:
    int rank_;
    int size_;
    std::vector<int> connections_;
    
    void initialize(const std::string& config_file);
    void setupConnections();
};
