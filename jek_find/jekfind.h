#include <limits.h>
#include <linux/limits.h>
#include <pthread.h>
#include <sys/stat.h>

typedef struct {
    int name;
    char *name_string;
    int contains;
    char *contains_string;
    int file;
    int directory;
    char *start_dir;
} Flags;

typedef struct Dir_Node {
    char path[PATH_MAX];
    struct Dir_Node *next;
} Dir_Node;

typedef struct {
    // pthread lock and conditions
    pthread_mutex_t lock;
    pthread_cond_t job_ready;
    pthread_cond_t traversal_complete;

    // FIFO Queue (linked list)
    Dir_Node *head;
    Dir_Node *tail;

    // Worker Flags
    int active_workers;
    int job_count;
    int stop;

    // Worker Context (for error handling)
    int worker_err;
} Job_Pool;

void pool_destructor(Job_Pool *pool);

int init_workers(Job_Pool *pool);

Job_Pool *init_pool(void);

void *run_worker(void *arg);

Dir_Node *get_job(Job_Pool *pool);

int add_job(Job_Pool *pool, char *path);

int handle_flags(int argc, char *argv[], Flags *flags);
