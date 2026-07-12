#include "jekfind.h"
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Usage:
// jekfind [start_dir] [options]
// Examples:
// jekfind . (print everything in working directory, and in those contained directories recursively)
// jekfind . --name foo (print any file or directory whose name is foo no case sensitivity)
// jekfind . --contains foo (print any file or directory whose name contains the string foo)
// jekfind . --type  f (same as [jekfind .], but only prints files)
// jekfind . --type d (same as [jekfind .], but only prints directories)
// --type and --name/--contains can be combined in any order

// Flags Struct:
//  int name;
//  char* name_string;
//  int contains;
//  char* contains_string;
//  int file;
//  int directory;
//  char* start_dir;

// Dir_Node struct:
//  char path[PATH_MAX];
//  struct Dir_Node *next;
//
//
// Job_Pool struct:
//  // pthread lock and conditions
//  pthread_mutex_t lock;
//  pthread_cond_t job_ready;
//  pthread_cond_t traversal_complete;
//
//  // FIFO Queue (linked list)
//  Dir_Node *head;
//  Dir_Node *tail;
//
//  // Worker Flags
//  int active_workers;
//  int job_count;
//  int stop;

Dir_Node *get_job(Job_Pool *pool) {
    if (pool->head == NULL) {
        return NULL;
    }
    Dir_Node *job = pool->head;
    // if the head node is the last element in the queue
    // we just want to set head to NULL after taking the job
    if (pool->head->next != NULL) {
        pool->head = pool->head->next;
    } else {
        pool->head = NULL;
    }

    return job;
}

int add_job(Job_Pool *pool, char *path) {
    // When we add jobs lets just put them as the tail of the Linked List
    // Then when we pop a job we just take the head
    Dir_Node *new_job = (Dir_Node *)malloc(sizeof(Dir_Node));
    if (new_job == NULL) {
        perror("error in add_job: ");
        return -1;
    }
    // String copy does not add null terminator for some reason??
    strncpy(new_job->path, path, PATH_MAX - 1);
    new_job->path[PATH_MAX - 1] = '\0';
    new_job->next = NULL;

    // Add new job to the end of the FIFO Linked List Queue

    // if queue is empty:
    if (pool->head == NULL) {
        pool->head = new_job;
        pool->tail = new_job;
    } else {
        pool->tail->next = new_job;
        pool->tail = new_job;
    }

    return 0;
}

void run_worker(void *arg) {}

int handle_flags(int argc, char *argv[], Flags *flags) {
    int opt;
    while ((opt = getopt(argc, argv, "n:c:t:"))) {
        switch (opt) {
        case 'n':
            flags->name = 1;
            flags->name_string = optarg;
            break;
        case 'c':
            flags->contains = 1;
            flags->contains_string = optarg;
            break;
        case 't':
            if (optarg[0] == 'f') {
                flags->file = 1;
            } else if (optarg[0] == 'd') {
                flags->directory = 1;
            } else {
                fprintf(stderr,
                        "Invalid argument for -t (f: files, d: directories)\n"
                        "do not use -t to get both: %s\n",
                        optarg);
                return -1;
            }
            break;
        default:
            fprintf(stderr, "Usage: jekfind [start_dir] [options]\n");
            return -1;
        }
        if ((flags->file == 0) && (flags->directory == 0)) {
            flags->file = 1;
            flags->directory = 1;
        }
        return 0;
    }

    return 0;
}

int recursive_chungus(char *dir_loc) {}

int start_traverse(Flags *flags) {}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: jekfind [start_dir] [options]");
        return 1;
    }
    // Initialize the flags struct with 0 values
    Flags *flags = calloc(1, sizeof(Flags));
    handle_flags(argc, argv, flags);
    // Now we can traverse from root and check each file/dir (depending on flags)
    // For the target we are looking for
    start_traverse(flags);
    return 0;
}
