#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

void create_config_file(const std::string& filename, int num_processes) {
    std::ofstream config(filename);
    config << num_processes << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <program> <num_processes>" << std::endl;
        return 1;
    }

    const char* program_path = argv[1];
    int num_processes = std::stoi(argv[2]);

    std::string config_file = "mympi_config.txt";
    create_config_file(config_file, num_processes);

    std::vector<pid_t> children;

    for (int rank = 0; rank < num_processes; ++rank) {
        pid_t pid = fork();
        
        if (pid == 0) {
            std::string rank_str = std::to_string(rank);
            execl(program_path, program_path, rank_str.c_str(), 
                  config_file.c_str(), nullptr);
            exit(1);
        } else if (pid > 0) {
            children.push_back(pid);
        } else {
            std::cerr << "Fork failed" << std::endl;
            return 1;
        }
    }

    for (pid_t pid : children) {
        waitpid(pid, nullptr, 0);
    }

    std::remove(config_file.c_str());
    return 0;
}
