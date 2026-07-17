# jek_wc

A reimplementation of [`wc(1)`](https://man7.org/linux/man-pages/man1/wc.1.html). Flags control which counts are shown; any combination is valid.

## Usage

```sh
jekwc [-b] [-l] [-w] <file>
```

| Flag | Effect |
|------|--------|
| `-b` | Print byte count |
| `-l` | Print line count |
| `-w` | Print word count |

## Syscalls used

| Syscall | Man page |
|---------|----------|
| `open` | [open(2)](https://man7.org/linux/man-pages/man2/open.2.html) |
| `read` | [read(2)](https://man7.org/linux/man-pages/man2/read.2.html) |
| `close` | [close(2)](https://man7.org/linux/man-pages/man2/close.2.html) |
