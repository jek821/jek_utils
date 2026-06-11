# C Tools

Reimplementations of standard Unix CLI tools written in C from scratch.

## Why

Most real-world projects are large enough that you spend most of your time thinking about architecture, abstractions, and coordination — not the code itself at a functional level. These small, self-contained programs flip that: each one is a focused exercise where the interesting work is in getting the low-level details right.

The secondary goal is building fluency with the Unix programming interface — syscalls, file descriptors, error handling, process semantics — as groundwork toward eventually contributing to the Linux kernel.

## Platform

These tools use POSIX syscalls (`open`, `read`, `write`, `close`) and will work on any Unix or Unix-like system: **Linux, macOS, and BSDs**. They will not work on Windows without a compatibility layer like WSL.

## Principles

Each tool is written with the following in mind:

- **Use the Unix API directly.** No wrappers around `open`, `read`, `write`, `close` — the point is to work at the interface, not above it.
- **Handle errors correctly.** Every syscall return value is checked. Partial writes are retried. Resources are cleaned up on failure paths.
- **Keep it simple and self-contained.** No unnecessary dependencies or abstractions beyond what the problem requires.

## Tools

| Tool | Description |
|------|-------------|
| [`jek_cat`](#jek_cat) | Reads a file and writes its contents to stdout, handling partial writes and read errors |
| [`jek_wc`](#jek_wc) | Counts the total number of bytes in a file and prints the result to stdout |

---

### jek_cat

A reimplementation of [`cat(1)`](https://man7.org/linux/man-pages/man1/cat.1.html).

**Usage**

```sh
jekcat <file>
```

**Syscalls used**

| Syscall | Man page |
|---------|----------|
| `open` | [open(2)](https://man7.org/linux/man-pages/man2/open.2.html) |
| `read` | [read(2)](https://man7.org/linux/man-pages/man2/read.2.html) |
| `write` | [write(2)](https://man7.org/linux/man-pages/man2/write.2.html) |
| `close` | [close(2)](https://man7.org/linux/man-pages/man2/close.2.html) |

---

### jek_wc

A reimplementation of [`wc(1)`](https://man7.org/linux/man-pages/man1/wc.1.html), currently supporting byte count.

**Usage**

```sh
jekwc <file>
```

**Syscalls used**

| Syscall | Man page |
|---------|----------|
| `open` | [open(2)](https://man7.org/linux/man-pages/man2/open.2.html) |
| `read` | [read(2)](https://man7.org/linux/man-pages/man2/read.2.html) |
| `write` | [write(2)](https://man7.org/linux/man-pages/man2/write.2.html) |

---

## Installing a tool

Each tool has a `Makefile`. To compile and install a tool so it's available anywhere on your system:

```sh
cd jek_cat
make install
```

This compiles the binary and copies it to `~/.local/bin/`. After that you can run it from any directory:

```sh
jekcat somefile.txt
```

**If your shell says "command not found"** after installing, `~/.local/bin` is not in your `PATH`. Add this line to your `~/.bashrc` (or `~/.zshrc` if you use zsh):

```sh
export PATH="$HOME/.local/bin:$PATH"
```

Then reload your shell with `source ~/.bashrc`.

**Other make commands**

| Command | Effect |
|---------|--------|
| `make` | Compile the binary (no install) |
| `make install` | Compile and install to `~/.local/bin` |
| `make uninstall` | Remove the binary from `~/.local/bin` |
| `make clean` | Remove the compiled binary from the project folder |

## Status

This is an ongoing project. More tools will be added over time.
