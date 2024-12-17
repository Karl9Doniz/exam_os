#!/usr/bin/env python3
import sys
import subprocess
import os

def create_config_file(mode, num_processes, config_file):
    """
    Create a configuration file with IP addresses and ports for processes.
    :param mode: Mode of operation (0: shared memory, 1: sockets)
    :param num_processes: Number of processes
    :param config_file: Path to configuration file
    """
    with open(config_file, 'w') as f:
        f.write(f"{mode}\n")
        f.write(f"{num_processes}\n")
        if mode == 0:
            shm_path = "/tmp/mympi_shm_key"
            open(shm_path, 'w').close()
            f.write(f"{shm_path}\n")
        else:
            for i in range(num_processes):
                f.write(f"127.0.0.1 {5000}\n")

def launch_processes(num_processes, config_file, program_path):
    """
    Launch processes using the provided configuration file and executable.
    :param num_processes: Number of processes to launch
    :param config_file: Path to config file
    :param program_path: Path to compiled program
    """
    processes = []
    for rank in range(num_processes):
        cmd = [program_path, str(rank), config_file]
        print(f"Launching process {rank}: {' '.join(cmd)}")
        proc = subprocess.Popen(cmd)
        processes.append(proc)

    for proc in processes:
        proc.wait()

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: launcher.py <shared_memory: 0 | sockets: 1> <num_processes> <program_path>")
        sys.exit(1)

    mode = int(sys.argv[1])
    num_processes = int(sys.argv[2])
    program_path = sys.argv[3]
    config_file = "config.txt"

    create_config_file(mode, num_processes, config_file)
    print(f"Configuration file '{config_file}' created.")

    launch_processes(num_processes, config_file, program_path)
    print("All processes have finished.")
