#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// Define optind (getopt() extern var) for flag handling
int optind;

typedef struct {
    int show_bytes;
    int show_lines;
    int show_words;
} Flags;

typedef struct {
    ssize_t file_byte_count;
    long file_line_count;
    long file_word_count;
} File_Data;

int handle_flags(const int *argc, char *argv[], Flags *flags) {
    int flag;
    while ((flag = getopt(*argc, argv, "blw")) != -1) {
        switch (flag) {
        case 'b':
            flags->show_bytes = 1;
            break;
        case 'l':
            flags->show_lines = 1;
            break;
        case 'w':
            flags->show_words = 1;
            break;
        case '?':
            fprintf(stderr, "Error Unknown Flag Detected");
            return -1;
        }
    }
    return 0;
}

int get_fd(char *fname) {
    int fd;
    if ((fd = open(fname, O_RDONLY)) == -1) {
        perror("Error Reading File");
        return -1;
    }
    return fd;
}

int main(int argc, char *argv[]) {
    // check for no flags or file
    if (argc < 2) {
        fprintf(stderr, "Usage: jekwc <flags> <file>");
    }
    Flags *flags = (Flags *)malloc(sizeof(Flags));
    int flag_result = handle_flags(&argc, argv, flags);

    if (flag_result == -1) {
        return 1;
    }

    char *file_name = argv[optind];

    int fd = get_fd(file_name);

    // initialize file data struct
    File_Data *file_data = (File_Data *)malloc(sizeof(File_Data));

    // free up allocated memory for flags and file data
    free(flags);
    free(file_data);
}