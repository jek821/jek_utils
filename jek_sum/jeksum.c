#include "jeksum.h"
#include <sodium.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Check Basic Usage (argc)
    if (argc < 2) {
        fprintf(stderr, "USAGE: ./jeksum [file_paths space separated]");
        return 1;
    }

    // Initialize Lib Sodium
    int init_libsod = sodium_init();
    if (init_libsod == -1) {
        fprintf(stderr, "Error Initializing Libsodium");
        return 1;
    }

    // Create Target Files struct and assign values
    Target_Files *Tf = (Target_Files *)malloc(sizeof(Target_Files));
    Tf->files = &argv[1];
    Tf->file_count = (size_t)(argv - 1);

    // Initialize Worker Threads
    int num_cpu = (int)sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[num_cpu];

    for (int i = 0; i < num_cpu; i++) {
    }
}

// memory tracker (mallocs to free)
// Target_Files *Tf
//
