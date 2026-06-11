# C Tools

Reimplementations of standard Unix CLI tools written in C from scratch.

## Why

Most real-world projects are large enough that you spend most of your time thinking about architecture, abstractions, and coordination — not the code itself at a functional level. These small, self-contained programs flip that: each one is a focused exercise where the interesting work is in getting the low-level details right.

The secondary goal is building fluency with the Unix programming interface — syscalls, file descriptors, error handling, process semantics — as groundwork toward eventually contributing to the Linux kernel.

## Principles

Each tool is written with the following in mind:

- **Use the Unix API directly.** No wrappers around `open`, `read`, `write`, `close` — the point is to work at the interface, not above it.
- **Handle errors correctly.** Every syscall return value is checked. Partial writes are retried. Resources are cleaned up on failure paths.
- **Keep it simple and self-contained.** No build systems, no dependencies, no abstractions beyond what the problem requires.

## Tools

| Tool | Description |
|------|-------------|
| `jek_cat` | Reads a file and writes its contents to stdout, handling partial writes and read errors |

## Building

Each tool lives in its own directory and compiles with a single command:

```sh
gcc -o <tool> <tool>.c
```

For example:

```sh
cd jek_cat
gcc -o jekcat jekcat.c
./jekcat somefile.txt
```

## Status

This is an ongoing project. More tools will be added over time.
