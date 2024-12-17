#include "mympi.hpp"
#include <iostream>

int main(int argc, char** argv) {
    try {
        MyMPI mpi(argc, argv);
        std::cout << "Process " << mpi.getRank() 
                  << " of " << mpi.getSize() 
                  << " started" << std::endl;
        
        // Simple ping between process 0 and 1
        if (mpi.getSize() >= 2) {
            if (mpi.getRank() == 0) {
                int data = 42;
                std::cout << "Process 0 sending " << data << std::endl;
                mpi.send(1, data);
            }
            else if (mpi.getRank() == 1) {
                int received = mpi.receive<int>(0);
                std::cout << "Process 1 received " << received << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
