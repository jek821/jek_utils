# Jek Utils

Reimplementations of standard Unix CLI tools written in C from scratch.

## Why

This isn't a loose grab-bag of Unix clones — it's a deliberate ladder. Each tool teaches a specific low-level skill (syscalls, file I/O, threading, wire protocols, cryptography, networking) that gets reused as a building block toward two eventual capstone projects:

- **jeksync** — a distributed file sync tool: walk a tree, hash and diff files, encrypt, and transfer changes over a custom wire protocol.
- **jekvpn** — a small mesh VPN with a coordination server, in the spirit of Tailscale/headscale: encrypted peer-to-peer tunnels plus a control plane that hands out peer maps.

See [`Docs/sync_plan.txt`](Docs/sync_plan.txt) and [`Docs/vpn_plan.txt`](Docs/vpn_plan.txt) for the full roadmaps — which tool teaches which skill, and how the two capstones share most of their trunk.

Along the way, each tool is also a focused exercise in getting the low-level details right without a framework or abstraction hiding them — building fluency with the Unix programming interface itself: syscalls, file descriptors, error handling, process semantics.

## Platform

These tools use POSIX syscalls (`open`, `read`, `write`, `close`) and will work on any Unix or Unix-like system: **Linux, macOS, and BSDs**. They will not work on Windows without a compatibility layer like WSL.

## Principles

Each tool is written with the following in mind:

- **Use the Unix API directly.** No wrappers around `open`, `read`, `write`, `close` — the point is to work at the interface, not above it.
- **Handle errors correctly.** Every syscall return value is checked. Partial writes are retried. Resources are cleaned up on failure paths.
- **No premature abstraction.** Each tool starts self-contained. Shared logic is extracted into [`libjek`](libjek/) *only once a second tool actually needs it* — the abstraction is discovered from two real callers, never designed up front. Duplication is cheaper than the wrong abstraction.

## Naming conventions

Every tool follows the same layout so the build and tooling work without per-tool special-casing:

- **Directory:** `jek_<tool>` — *with* an underscore (e.g. `jek_cat`, `jek_wc`, `jek_ls`).
- **Source, header, and binary:** `jek<tool>` — *no* underscore (e.g. `jekcat.c`, `jekwc.h`, the `jekls` binary).

So a tool named `foo` lives in `jek_foo/` and contains:

| File | Purpose |
|------|---------|
| `jekfoo.c` | Source |
| `jekfoo.h` | Header with prototypes (and any shared types) |
| `Makefile` | Builds `jekfoo`, with `install` / `uninstall` / `clean` / `test` targets |
| `test.sh` | Test suite, runnable via `make test` |
| `README.md` | Usage, flags, and syscalls/APIs used |

The compiled binary is `jek_foo/jekfoo` (no extension). The root `.gitignore` relies on this convention — it ignores `jek_*/jek*` while keeping `*.c` / `*.h` / `*.sh`, so build artifacts are never committed and **no `.gitignore` edits are needed when you add a new tool.**

## Shared library (`libjek`)

The project is organized in three layers, not as a flat pile of tools:

1. **[`libjek/`](libjek/)** — the shared core. Non-trivial logic that more than one consumer needs (file-tree traversal, buffered/atomic I/O, streaming hashes, the wire codec, the crypto session, the thread pool) lives here once, tested in isolation.
2. **The tools** (`jek_find/`, `jek_sum/`, …) — thin CLIs over `libjek`. They stay as focused learning exercises *and* double as the tests that exercise each library module.
3. **The capstones** (`jeksync`, `jekvpn`) — compose `libjek`, not the tools.

Code reaches `libjek` by **organic extraction, not up-front design**: a thing is pulled into the library once a *second* tool actually needs it, so the API is shaped by two real callers instead of guessed at in advance. `libjek` is a destination, not a scheduled phase — see [`libjek/README.md`](libjek/README.md) for what belongs there and the module convention.

## Tools

| Tool | Description |
|------|-------------|
| [`jek_cat`](jek_cat/README.md) | Reads a file and writes its contents to stdout, handling partial writes and read errors |
| [`jek_wc`](jek_wc/README.md) | Counts bytes, lines, and/or words in a file, selected by flags |
| [`jek_ls`](jek_ls/README.md) | Lists the contents of the current directory, with modification times and color for subdirectories |

`jek_find` and `jek_sum` are in progress and will be added here once they're functional.

## Build system

The repo uses a two-level Makefile structure:

- **Root `Makefile`** — orchestrates all tools from one place. Run from the repo root.
- **Per-tool `Makefile`** — builds and installs that tool in isolation. Run from inside a tool's directory.

Both support the same targets (`all`, `install`, `uninstall`, `clean`, `test`). The root one just delegates each target down to every tool's Makefile using `make -C <dir>`.

Once shared logic is extracted, [`libjek/`](libjek/) builds to a static archive `libjek.a`; tools and capstones link it by adding the archive to their link line plus `-I../libjek` for the headers. That Makefile wiring lands with the first real extraction — until then there is nothing to build there.

### Install everything at once

```sh
make install
```

This builds and installs all tools to `~/.local/bin/`.

### Work on a single tool

```sh
cd jek_cat
make install
```

Useful when you only want to build or reinstall one tool without touching the others.

### All make targets

| Command | Effect |
|---------|--------|
| `make` | Compile all tools (no install) |
| `make install` | Compile and install all tools to `~/.local/bin` |
| `make uninstall` | Remove all tools from `~/.local/bin` |
| `make clean` | Remove compiled binaries from all project folders |
| `make test` | Build and run each tool's test suite (tools without tests are skipped) |

The per-tool Makefiles support the same commands from inside the tool's directory.

### Compiling directly (strict checks)

The Makefiles compile every tool with a strict warning set so mistakes are caught at compile time rather than at runtime. To compile a tool by hand with the same checks:

```sh
gcc -std=c11 -D_DEFAULT_SOURCE \
    -Wall -Wextra -Wpedantic -Wshadow \
    -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Werror \
    -o jekwc jekwc.c
```

- `-std=c11 -Wpedantic` — strict ISO C11, no silent compiler extensions.
- `-D_DEFAULT_SOURCE` — **required.** Strict ISO mode hides the POSIX + BSD APIs (`getopt`, `read`, `struct dirent`'s `d_type`/`DT_*`, `st_mtim`, …); this macro exposes them. It is a superset of `-D_POSIX_C_SOURCE=200809L`, so every tool shares the one flag. Without it the build fails with errors like "implicit declaration of `getopt`" or "`DT_DIR` undeclared".
- `-Wall -Wextra -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wconversion` — the warning set.
- `-Werror` — every warning is a build failure.

### Editor / clangd setup

`compile_flags.txt` at the repo root hands these same flags to [clangd](https://clangd.llvm.org/) (the LSP behind most editors' C support), so your editor sees the same declarations the build does. Without it, clangd falls back to plain ISO C and wrongly flags POSIX/BSD symbols like `getopt` or `DT_DIR` as undeclared — even though `make` compiles fine. The file is plain text, needs no tooling installed, and is committed so it travels with the repo; an editor with no clangd simply gets no inline diagnostics (the build is unaffected).

**If your shell says "command not found"** after installing, `~/.local/bin` is not in your `PATH`. Add this line to your `~/.bashrc` (or `~/.zshrc` if you use zsh):

```sh
export PATH="$HOME/.local/bin:$PATH"
```

Then reload your shell with `source ~/.bashrc`.

## Status

This is an ongoing project. More tools will be added over time.
