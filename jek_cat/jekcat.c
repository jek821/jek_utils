#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int write_all_bytes(int output_fd, const char *buffer, ssize_t bytes_to_write) {
    ssize_t total_bytes_written = 0;

    while (total_bytes_written < bytes_to_write) {

        ssize_t bytes_written =
            write(output_fd, buffer + total_bytes_written, bytes_to_write - total_bytes_written);
        if (bytes_written == -1) {
            perror("write");
            return 1;
        }
        total_bytes_written += bytes_written;
    }
    return 0;
}

int copy_fd_to_stdout(int input_fd) {
    char buffer[4096];
    ssize_t bytes_read;

    while (1) {
        bytes_read = read(input_fd, buffer, sizeof(buffer));

        if (bytes_read == -1) {
            perror("read");
            close(input_fd);
            return 1;
        }

        if (bytes_read == 0) {
            close(input_fd);
            return 0;
        }

        int write_status = write_all_bytes(STDOUT_FILENO, buffer, bytes_read);

        if (write_status != 0) {
            close(input_fd);
            return 1;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "usage: jekcat <file>\n");
        return 1;
    }

    int input_fd = open(argv[1], O_RDONLY);

    if (input_fd == -1) {
        perror("open");
        return 1;
    }

    return copy_fd_to_stdout(input_fd);
}