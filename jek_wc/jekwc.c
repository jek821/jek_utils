#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int get_fd(char *file_name) {

    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    return fd;
}

ssize_t get_bytes(int fd) {
    char buffer[4096];
    ssize_t total_bytes = 0;

    while (1) {
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read == 0) {
            return total_bytes;
        }
        if (bytes_read == -1) {
            perror("read");
            return 1;
        }
        total_bytes += bytes_read;
    }
}

int write_bytecount(int output_fd, ssize_t byte_count) {
    char buffer[10];
    int string_len = snprintf(buffer, sizeof(buffer), "%d", byte_count);
    ssize_t write_result = write(output_fd, buffer, string_len);
    if (write_result == -1) {
        perror("write");
        return -1;
    }
    write_result = write(output_fd, "\n", 1);
    if (write_result == -1) {
        perror("write");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "usage: jekwc <filename>");
        return 1;
    }
    int fd = get_fd(argv[1]);
    if (fd == 1) {
        return 1;
    }
    int fd_bytes = get_bytes(fd);

    int write_status = write_bytecount(STDOUT_FILENO, fd_bytes);
    if (write_status == -1) {
        return 1;
    }
    return 0;
}