#include "jekls.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#define BLUE "\033[34m"

int fd_to_buff(char *fn, struct stat *buff) {
    int stat_result = stat(fn, buff);
    if (stat_result == -1) {
        perror("error stat'ing file");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    // Arg Checks Later (-a for hidden files?)

    // get current dir
    DIR *current_dir = opendir(".");
    if (current_dir == NULL) {
        perror("error opening current directory");
        return 1;
    }

    struct stat *buff;
    buff = malloc(sizeof(struct stat));

    struct tm *local_time;
    char buffer[64];

    // iterate over dir contents
    struct dirent *current_dir_element;
    while ((current_dir_element = readdir(current_dir)) != NULL) {
        fd_to_buff(current_dir_element->d_name, buff);
        if (current_dir_element->d_type == DT_DIR) {
            printf(BLUE "%s\n", current_dir_element->d_name);
        } else {
            printf("%s\n", current_dir_element->d_name);
        }
        local_time = localtime(&buff->st_mtim.tv_sec);

        // Format: "2026-06-17 00:07:00"
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);

        printf("Formatted time: %s\n", buffer);
    }
    return 0;
}
