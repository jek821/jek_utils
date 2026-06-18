#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
    int show_bytes;
    int show_lines;
    int show_words;
} Options;

int handle_flags(const int argc, char *argv[], Options *opts) {
    int opt;

    while ((opt = getopt(argc, argv, "blw")) != -1) {
        switch (opt) {
        case 'b':
            opts->show_bytes = 1;
            break;

        case 'l':
            opts->show_lines = 1;
            break;

        case 'w':
            opts->show_words = 1;
            break;

        case '?':
            return -1;

        default:
            return -1;
        }
    }

    return optind;
}

int get_fd(const char *file_name) {
    int fd = open(file_name, O_RDONLY);

    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }

    return fd;
}

long get_lines(const char *buffer, ssize_t num_bytes) {
    long line_count = 0;

    for (ssize_t i = 0; i < num_bytes; i++) {
        if (buffer[i] == '\n') {
            line_count++;
        }
    }

    return line_count;
}

int is_space(char c) { return isspace((unsigned char)c); }

long get_words(const char *buffer, ssize_t num_bytes, int *in_word) {
    long word_count = 0;

    for (ssize_t i = 0; i < num_bytes; i++) {
        if (!is_space(buffer[i])) {
            if (*in_word == 0) {
                word_count++;
            }

            *in_word = 1;
        } else {
            *in_word = 0;
        }
    }

    return word_count;
}

int get_data(int fd, ssize_t *bytes, long *lines, long *words, const Options *opts) {
    int in_word = 0;
    ssize_t bytes_read;
    char buffer[4096];

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) != 0) {
        if (bytes_read == -1) {
            perror("Error reading file");
            return -1;
        }

        if (opts->show_bytes) {
            *bytes += bytes_read;
        }

        if (opts->show_lines) {
            *lines += get_lines(buffer, bytes_read);
        }

        if (opts->show_words) {
            *words += get_words(buffer, bytes_read, &in_word);
        }
    }

    return 0;
}

void show_results(ssize_t bytes, long lines, long words, const Options *opts) {
    if (opts->show_bytes) {
        printf("Bytes: %zd\n", bytes);
    }

    if (opts->show_lines) {
        printf("Lines: %ld\n", lines);
    }

    if (opts->show_words) {
        printf("Words: %ld\n", words);
    }
}

int main(int argc, char *argv[]) {
    Options opts = {0, 0, 0};

    int file_index = handle_flags(argc, argv, &opts);

    if (file_index == -1) {
        return 1;
    }

    if (file_index >= argc) {
        fprintf(stderr, "Usage: %s [-b] [-l] [-w] FILE\n", argv[0]);
        return 1;
    }

    int fd = get_fd(argv[file_index]);

    if (fd == -1) {
        return 1;
    }

    ssize_t bytes = 0;
    long lines = 0;
    long words = 0;

    int data_status = get_data(fd, &bytes, &lines, &words, &opts);

    if (close(fd) == -1) {
        perror("Error closing file");
        return 1;
    }

    if (data_status == -1) {
        return 1;
    }

    show_results(bytes, lines, words, &opts);

    return 0;
}