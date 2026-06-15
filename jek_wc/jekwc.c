#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

extern int optind;

typedef struct {
  int show_bytes;
  int show_lines;
  int show_words;

} Options;

int handle_flags(const int argc, char *argv[], Options *flags) {
  int opt;
  while ((opt = getopt(argc, argv, "blw")) != -1) {
    switch (opt) {
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

int get_fd(char *file_name) {
  int fd = open(file_name, O_RDONLY);
  if (fd == -1) {
    perror("Error Opening File");
    return -1;
  }

  return fd;
};

long get_lines(char *buffer, ssize_t num_bytes) {
  long line_count = 0;
  for (long i = 0; i <= num_bytes - 1; i++) {
    if (buffer[i] == 0b00001010) {
      line_count++;
    }
  }
  return line_count;
}

int is_space(char c) {
  int is_space = (c == 0b00001101 || c == 0b00001001 || c == 0b00100000 ||
                  c == 0b00001010);
  return is_space;
}

// hello hello

long get_words(char *buffer, ssize_t num_bytes, int *in_word) {
  long word_count = 0;
  for (long i = 0; i <= num_bytes - 1; i++) {
    if (is_space(buffer[i]) == 0) {
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

int get_data(int fd, ssize_t *bytes, long *lines, long *words, Options *opts) {
  int in_word = 0;
  ssize_t bytes_read;
  char buffer[4096];
  while ((bytes_read = read(fd, buffer, sizeof(buffer))) != 0) {
    if (bytes_read == -1) {
      perror("Error Reading File");
      return -1;
    }

    // Could thread
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

void show_results(ssize_t bytes, long lines, long words, Options *opts) {
  if (opts->show_bytes) {
    printf("Bytes: %zu\n", bytes);
  }
  if (opts->show_lines) {
    printf("Lines: %ld\n", lines);
  }
  if (opts->show_words) {
    printf("Words: %ld\n", words);
  }
}

int main(int argc, char *argv[]) {
  Options opts;
  int flag_count = handle_flags(argc, argv, &opts);

  int fd = get_fd(argv[flag_count + 1]);

  if (fd == -1) {
    return 1;
  }

  ssize_t bytes;
  long lines;
  long words;

  int data_status = get_data(fd, &bytes, &lines, &words, &opts);

  if (close(fd) == -1) {
    perror("Error Closing File");
    return 1;
  }

  if (data_status == -1) {
    return 1;
  }

  show_results(bytes, lines, words, &opts);
  return 0;
}