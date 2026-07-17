#include <limits.h>
#include <linux/limits.h>
#include <pthread.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct {
    // Null Terminator separated paths for each target file
    char **files;
    size_t file_count;
} Target_Files;

// need to state type above as well since LL nodes are self referencing
typedef struct Dir_Node {
    char path[PATH_MAX];
    struct Dir_Node *next;
} Dir_Node;

// Pool for managing threaded work
typedef struct {
    // Mutex lock and thread conditions
    pthread_mutex_t lock;
    pthread_cond_t job_ready;
    pthread_cond_t hashing_complete;

    // Job Queue (FIFO Linked List)
    Dir_Node *head;
    Dir_Node *tail;

    // Shared Worker Flags
    int active_workers;
    int job_count;
    int stop;

    // worker context to handle errors in threads
    int worker_err;
} Job_Pool;
