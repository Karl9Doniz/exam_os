#ifndef MYMPI_H
#define MYMPI_H

#include <string>

class MyMPI {
public:
    MyMPI(int argc, char** argv);
    ~MyMPI();

    int get_world_size() const;
    int get_rank() const;

    void send(int dest, const void* data, size_t size);
    void receive(int source, void* buffer, size_t size);
    void barrier();

private:
    int rank;
    int world_size;
    int server_fd;
    int client_fd;
    int* connections;
    void setup_server(const std::string& ip, int port);
    void setup_client(const std::string& server_ip, int port);
};

#endif
