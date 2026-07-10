typedef struct {
    int name;
    char *name_string;
    int contains;
    char *contains_string;
    int file;
    int directory;
    char *start_dir;
} Flags;

int handle_flags(int argc, char *argv[], Flags *flags);
