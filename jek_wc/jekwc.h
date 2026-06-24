#ifndef JEKWC_H
#define JEKWC_H

#include <stddef.h>    // size_t
#include <sys/types.h> // ssize_t

// Which counts the user asked for (set from the command-line flags).
typedef struct {
    int show_bytes;
    int show_words;
    int show_lines;
} Flags;

// Running totals accumulated across the file.
typedef struct {
    size_t byte_count;
    long word_count;
    long line_count;
} File_Data;

void usage_error(void);
int handle_flags(const int *argc, char *argv[], Flags *flags);
Flags *initialize_flags(void);
int get_fd(char *file_name);
long lines_from_buff(char *buffer, ssize_t bytes_to_read);
int is_space(char byte);
long words_from_buff(char *buffer, ssize_t bytes_to_read, int *in_word);
int get_data(int fd, File_Data *file_data, Flags *flags);
void print_data(Flags *flags, File_Data *file_data);

#endif
