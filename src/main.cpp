#include "mympi.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {
    MyMPI mpi_manager(argc, argv);

    int rank = mpi_manager.get_rank();
    int world_size = mpi_manager.get_world_size();

    if (world_size < 2) {
        std::cerr << "This example requires at least 2 processes.\n";
        return 1;
    }

    const int PRODUCER_RANK = 0;
    int buffer = -1;

    if (rank == PRODUCER_RANK) {
        for (int i = 1; i <= 5; ++i) {
            buffer = i * 10;
            std::cout << "[Producer] Produced: " << buffer << std::endl;
            for (int dest = 1; dest < world_size; dest++) {
                mpi_manager.send(dest, &buffer, sizeof(int));
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } else {
        for (int i = 0; i < 5; ++i) {
            mpi_manager.receive(0, &buffer, sizeof(int));
            std::cout << "[Consumer " << rank << "] Received: " << buffer << std::endl;
        }
    }

    mpi_manager.barrier();
    return 0;
}