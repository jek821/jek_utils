#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int optind;

typedef struct {
    int show_bytes;
    int show_words;
    int show_lines;
} Flags;

typedef struct {
    size_t byte_count;
    long word_count;
    long line_count;
} File_Data;

int usage_error() {
    fprintf(stderr, "==================================================\n");
    fprintf(stderr, "USAGE ERROR:\nCorrect Usage: ./jekwc <flags> <file>\n");
    fprintf(stderr, "Flags:\n-b (print number of bytes)\n-w (print number of words)\n-l "
                    "(print number of lines)\nNo flags results in all values being "
                    "printed\n");
    fprintf(stderr, "==================================================\n");
}

int handle_flags(const int *argc, char *argv[], Flags *flags) {
    int opt;

    while ((opt = getopt(*argc, argv, "bwl")) != -1) {
        switch (opt) {
        case 'b':
            flags->show_bytes = 1;
            printf("-b flag detected!\n");
            break;
        case 'w':
            flags->show_words = 1;
            printf("-w flag detected!\n");
            break;
        case 'l':
            flags->show_lines = 1;
            printf("-l flag detected!\n");
            break;
        case '?':
            usage_error();
            return -1;
        }
    }
    if (optind == 1) {
        flags->show_words = 1;
        flags->show_bytes = 1;
        flags->show_lines = 1;
    }
}

Flags *initialize_flags() {
    Flags *flags = (Flags *)malloc(sizeof(Flags));
    flags->show_bytes = 0;
    flags->show_lines = 0;
    flags->show_words = 0;
    return flags;
}

int get_fd(char *file_name) {
    int fd;

    if ((fd = open(file_name)) == -1) {
        perror("Error Opening File");
        return -1;
    }
    return fd;
}

long lines_from_buff(char *buffer, ssize_t bytes_to_read) {
    long line_count = 0;

    for (ssize_t i = 0; i < bytes_to_read; i++) {
        if (buffer[i] == '\n') {
            line_count++;
        }
    }

    return line_count;
}

int is_space(char byte) { return byte = }

long words_from_buff(char *buffer, ssize_t bytes_to_read, int *in_word) {
    long word_count = 0;

    for (ssize_t i; i < bytes_to_read; i++) {
        if
    }
}

int get_data(int fd, File_Data *file_data, Flags *flags) {
    char byte_buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, byte_buffer, sizeof(byte_buffer))) != 0) {
        if (flags->show_bytes) {
            file_data->byte_count += bytes_read;
        }
        if (flags->show_lines) {
            file_data->line_count += lines_from_buff(byte_buffer, bytes_read);
        }
        if (flags->show_words) {
            int in_word = 0;
            file_data->word_count += words_from_buff(byte_buffer, bytes_read, &in_word);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    Flags *flags = initialize_flags();
    File_Data *file_data = (File_Data *)malloc(sizeof(File_Data));

    // Handle flags
    int handle_flag_result = handle_flags(&argc, argv, flags);
    printf("%d\n", argc);
    printf("%d\n", optind);

    // Handle file (get file descriptor)
    int fd = get_fd(argv[optind]);
    // Collect data from file (fill File_Data struct)

    // Send file data to STDOUT
}