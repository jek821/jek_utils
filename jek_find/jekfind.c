#include "jekfind.h"
#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Usage:
// jekfind [start_dir] [options]
// Examples:
// jekfind . (print everything in working directory, and in those contained directories recursively)
// jekfind . --name foo (print any file or directory whose name is foo no case sensitivity)
// jekfind . --contains foo (print any file or directory whose name contains the string foo)
// jekfind . --type  f (same as [jekfind .], but only prints files)
// jekfind . --type d (same as [jekfind .], but only prints directories)
// --type and --name/--contains can be combined in any order

// Flags Struct:
//  int name;
//  char* name_string;
//  int contains;
//  char* contains_string;
//  int file;
//  int directory;
//  char* start_dir;

int handle_flags(int argc, char *argv[], Flags *flags) {
    int opt;
    while ((opt = getopt(argc, argv, "n:c:t:"))) {
        switch (opt) {
        case 'n':
            flags->name = 1;
            flags->name_string = optarg;
            break;
        case 'c':
            flags->contains = 1;
            flags->contains_string = optarg;
            break;
        case 't':
            if (optarg[0] == 'f') {
                flags->file = 1;
            } else if (optarg[0] == 'd') {
                flags->directory = 1;
            } else {
                fprintf(stderr, "Invalid argument for --type: %s\n", optarg);
                return -1;
            }
            break;
        default:
            fprintf(stderr, "Usage: jekfind [start_dir] [options]\n");
            return -1;
        }
        if ((flags->file == 0) && (flags->directory == 0)) {
            flags->file = 1;
            flags->directory = 1;
        }
        return 0;
    }

    return 0;
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: jekfind [start_dir] [options]");
        return 1;
    }
    // Initialize the flags struct with 0 values
    Flags *flags = calloc(1, sizeof(Flags));
    handle_flags(argc, argv, flags);
    return 0;
}
