#include <limits.h>
#include <linux/limits.h>
#include <pthread.h>

typedef struct {
    int name;
    char *name_string;
    int contains;
    char *contains_string;
    int file;
    int directory;
    char *start_dir;
} Flags;

typedef struct Dir_Node{
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

} Job_Pool;



int handle_flags(int argc, char *argv[], Flags *flags);

int print_entry(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
