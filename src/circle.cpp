#include "mympi.h"
#include <iostream>
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

    int next_rank = (rank + 1) % world_size;
    int prev_rank = (rank - 1 + world_size) % world_size;

    const int NUM_ROUNDS = 2;
    
    int token;

    if (rank == 0) {
        std::cout << "Process 0: Starting circle communication" << std::endl;
        
        for (int round = 1; round <= NUM_ROUNDS; ++round) {
            token = round * 100;
            std::cout << "Process 0: Sending token " << token << " (Round " << round << ")" << std::endl;
            mpi_manager.send(next_rank, &token, sizeof(int));

            mpi_manager.receive(prev_rank, &token, sizeof(int));
            std::cout << "Process 0: Received token " << token << " back (Round " << round << ")" << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } else {
        for (int round = 1; round <= NUM_ROUNDS; ++round) {
            mpi_manager.receive(prev_rank, &token, sizeof(int));
            std::cout << "Process " << rank << ": Received token " << token 
                      << " from Process " << prev_rank << " (Round " << round << ")" << std::endl;

            token += rank;

            std::cout << "Process " << rank << ": Forwarding token " << token 
                      << " to Process " << next_rank << " (Round " << round << ")" << std::endl;
            mpi_manager.send(next_rank, &token, sizeof(int));
        }
    }

    mpi_manager.barrier();
    std::cout << "Process " << rank << ": Finished circle communication" << std::endl;

    return 0;
}
