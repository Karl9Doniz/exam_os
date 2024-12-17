#pragma once

#include <string>

const int MAX_PROCESSES = 32;
const int MAX_MESSAGE_SIZE = 1024;

struct SharedMemorySegment {
    int shmid;
    void* addr;
};

struct SharedMemoryHeader {
    bool message_ready[MAX_PROCESSES][MAX_PROCESSES];
    char data[MAX_PROCESSES][MAX_PROCESSES][MAX_MESSAGE_SIZE];
    size_t message_sizes[MAX_PROCESSES][MAX_PROCESSES];
    bool barrier_flags[MAX_PROCESSES];
};

class MyMPI {
private:
    int rank;
    int world_size;
    bool use_shared_memory;
    
    int server_fd;
    int client_fd;
    int* connections;

    SharedMemorySegment shm_segment;
    SharedMemoryHeader* shm_header;

    void setup_server(const std::string& ip, int port);
    void setup_client(const std::string& server_ip, int port);
    void setup_shared_memory(const std::string& segment_name);

public:
    MyMPI(int argc, char** argv);
    ~MyMPI();

    void send(int dest, const void* data, size_t size);
    void receive(int source, void* buffer, size_t size);
    void barrier();

    int get_world_size() const;
    int get_rank() const;
};
