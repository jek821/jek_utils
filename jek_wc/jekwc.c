#include <stdio.h>
#include <unistd.h>

extern int optind;

typedef struct
{
    int show_bytes;
    int show_lines;
    int show_words;

} Options;

int handle_flags(const int argc, char *argv[], Options *flags)
{
    int opt;
    while ((opt = getopt(argc, argv, "blw")) != -1)
    {
        switch (opt)
        {
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
            return -1;

        default:
        }
    }
    return optind - 1;
}

int main(int argc, char *argv[])
{
    Options flags;
    int optind = handle_flags(argc, argv, &flags);
    printf("%d", optind);
}