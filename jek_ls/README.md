# jek_ls

A reimplementation of [`ls(1)`](https://man7.org/linux/man-pages/man1/ls.1.html). Lists entries in the current directory, printing each entry's name and last modification time. Directory entries are highlighted in blue.

## Usage

```sh
jekls
```

## APIs used

| Function | Man page |
|----------|----------|
| `opendir` | [opendir(3)](https://man7.org/linux/man-pages/man3/opendir.3.html) |
| `readdir` | [readdir(3)](https://man7.org/linux/man-pages/man3/readdir.3.html) |
| `stat` | [stat(2)](https://man7.org/linux/man-pages/man2/stat.2.html) |
