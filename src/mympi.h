#pragma once

#include <string>
#include <queue>
#include <future>
#include <optional>

const int MAX_PROCESSES = 32;
const int MAX_MESSAGE_SIZE = 1024;

struct Request {
    int source;
    int dest;
    void* buffer;
    size_t size;
    bool completed;
    std::promise<void> promise;
};

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

    Request* asend(int dest, const void* data, size_t size);
    Request* arecv(int source, void* buffer, size_t size);
    void wait(Request* request);
    bool test(Request* request);

public:
    MyMPI(int argc, char** argv);
    ~MyMPI();

    void send(int dest, const void* data, size_t size);
    void receive(int source, void* buffer, size_t size);
    void barrier();

    std::queue<Request*> pending_requests;
    std::mutex requests_mutex;
    
    void check_completion(Request* request);

    int get_world_size() const;
    int get_rank() const;
};
