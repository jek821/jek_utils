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
    while ((opt = getopt(argc, argv, "n:c:t:")))
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: jekfind [start_dir] [options]");
        return 1;
    }
    // Initialize the flags struct with 0 values
    Flags *flags = calloc(1, sizeof(Flags));

    return 0;
}
