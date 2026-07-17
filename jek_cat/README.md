# jek_cat

A reimplementation of [`cat(1)`](https://man7.org/linux/man-pages/man1/cat.1.html). Reads a file and writes its contents to stdout, handling partial writes and read errors.

## Usage

```sh
jekcat <file>
```

## Syscalls used

| Syscall | Man page |
|---------|----------|
| `open` | [open(2)](https://man7.org/linux/man-pages/man2/open.2.html) |
| `read` | [read(2)](https://man7.org/linux/man-pages/man2/read.2.html) |
| `write` | [write(2)](https://man7.org/linux/man-pages/man2/write.2.html) |
| `close` | [close(2)](https://man7.org/linux/man-pages/man2/close.2.html) |
