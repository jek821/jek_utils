# jekfind syscall / libc reference

How to walk a file tree from a starting directory using `opendir` / `readdir` / `lstat` + recursion (no `nftw`), and how to work out where you are relative to the system root (`/`). Covers the directory-stream primitives, `struct stat`, and the path helpers (`getcwd`, `realpath`, `dirname`/`basename`).

## 1. The manual tree walk: `opendir` / `readdir` / `lstat` + recursion

This is the core of jekfind. You open a directory, read its entries one at a time, and recurse into any entry that is itself a directory. jek_ls already uses `opendir`/`readdir`/`stat` for a single directory — this is the same primitives plus recursion.

```c
#include <dirent.h>   // opendir, readdir, closedir, struct dirent
#include <sys/stat.h> // lstat, struct stat, S_ISDIR
#include <string.h>   // strcmp
#include <stdio.h>    // snprintf, perror
#include <limits.h>   // PATH_MAX

void walk(const char *path) {
    DIR *d = opendir(path);
    if (!d) { perror(path); return; }   // e.g. permission denied; skip it

    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        // ALWAYS skip "." and ".." or you recurse forever.
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0)
            continue;

        // Build the child path: parent + "/" + name.
        char child[PATH_MAX];   // PATH_MAX from <limits.h>
        snprintf(child, sizeof child, "%s/%s", path, e->d_name);

        struct stat sb;
        // lstat, not stat, so symlinks aren't followed (no loops).
        if (lstat(child, &sb) == -1) { perror(child); continue; }

        printf("%s\n", child);      // <-- where jekfind decides to print or not

        if (S_ISDIR(sb.st_mode))
            walk(child);            // recurse into subdirectory
    }
    closedir(d);
}
```

### The three primitives

**`opendir`** — open a directory for reading. Returns a `DIR *` stream, or `NULL` on error (`errno` set — e.g. `EACCES` if you can't read it). Always check for `NULL`; a find-like tool hits unreadable directories all the time and should skip them, not crash.

```c
DIR *opendir(const char *name);
```

**`readdir`** — return the next entry in the stream, one call at a time, or `NULL` when there are no more entries (or on error). You do NOT free the returned pointer; it points into storage owned by the `DIR` stream and is overwritten on the next `readdir` call.

```c
struct dirent *readdir(DIR *dirp);
```

`struct dirent` — the fields you can portably rely on:

| Field | Meaning |
|-------|---------|
| `d_name` | The entry's filename (basename only — no path). This is the one you'll use most. |
| `d_type` | Entry type *hint*: `DT_DIR`, `DT_REG`, `DT_LNK`, `DT_UNKNOWN`, … Can save a `stat()` call, BUT some filesystems always return `DT_UNKNOWN`, so you still need an `lstat` fallback. |
| `d_ino` | Inode number. |

**`closedir`** — close the stream and free it. One call per successful `opendir`. Forgetting this leaks a file descriptor per directory, and with deep recursion you'll run out.

```c
int closedir(DIR *dirp);
```

### Key points that bite people

- `readdir` returns `.` and `..` — you **MUST** skip them, or `walk(child)` recurses into `.` forever.
- Order is **not** sorted — entries come back in whatever order the filesystem stores them. If you want sorted output, collect names and `qsort` them (or look at `scandir`, which does both in one call).
- Use `lstat`, not `stat`, when deciding whether to recurse — otherwise a symlink pointing back up the tree sends you into an infinite loop. `lstat` reports the *link itself*, so a symlinked directory shows up as a symlink (not a dir) and you won't descend into it.
- Build paths with `snprintf` (not `strcpy`/`strcat`) so an over-long name can't overflow `child`. `snprintf` truncates instead. If you want to be strict, check its return value against `sizeof child` and skip paths that would truncate.
- `readdir` returning `NULL` means "end of directory" OR "error". If you need to tell them apart, set `errno = 0` before the call and check it after. For jekfind, treating `NULL` as "done" is fine.
- Recursion depth = tree depth. Very deep trees can blow the C stack; for jekfind's scale that's not a concern, but it's why production tools sometimes use an explicit stack instead.

### How the flags map onto the walk

At the `printf` line above, jekfind decides whether to actually print `child`, using the `Flags` struct:

- `-t f` (`flags->file`) → only print when `S_ISREG(sb.st_mode)`.
- `-t d` (`flags->directory`) → only print when `S_ISDIR(sb.st_mode)`.
- neither set → print both (your default in `handle_flags`).
- `-n foo` (`flags->name`) → print only if `strcasecmp(e->d_name, flags->name_string) == 0` (case-insensitive exact match; `strcasecmp` from `<strings.h>`).
- `-c foo` (`flags->contains`) → print only if `strstr(e->d_name, flags->contains_string) != NULL` (substring match).

Note the type test (recurse) and the print test are separate: with `-t f` you still recurse into directories, you just don't *print* them. Match against `e->d_name` (the basename), not the full `child` path, so `-n main.c` doesn't accidentally match a directory name in the path.

Unlike `nftw`, a hand-written `walk` can take the `Flags *` as a parameter (or you pass the fields you need) — no need for a file-scope global to smuggle context into a callback.

## 2. `struct stat` — what you learn about each entry

```c
#include <sys/stat.h>
struct stat sb;
stat(path, &sb);    // follows symlinks
lstat(path, &sb);   // does NOT follow symlinks (reports the link itself)
fstat(fd,   &sb);   // for an already-open file descriptor
```

**Useful fields**

| Field | Meaning |
|-------|---------|
| `sb.st_mode` | Type + permission bits. Test the type with the `S_IS*` macros: `S_ISREG(sb.st_mode)` regular file, `S_ISDIR(sb.st_mode)` directory, `S_ISLNK(sb.st_mode)` symlink (only meaningful after `lstat`). |
| `sb.st_size` | Size in bytes (files). |
| `sb.st_mtime` | Last modification time (seconds). `st_mtim.tv_nsec` for ns. (jek_ls already prints this field.) |
| `sb.st_ino` | Inode number. (`st_dev`, `st_ino`) together uniquely identify a file — how you'd detect hardlinks or a symlink that loops back onto an ancestor. |

**Type macros you'll actually use in the walk**

| Macro | True when the entry is a… |
|-------|---------------------------|
| `S_ISREG(sb.st_mode)` | regular file (`-t f`) |
| `S_ISDIR(sb.st_mode)` | directory (`-t d`, and what triggers recursion) |
| `S_ISLNK(sb.st_mode)` | symlink (only after `lstat`; `stat` would report the target's type) |

## 3. "Where am I relative to `/`" — absolute paths and the root

**`getcwd`** — the process's current working directory as an absolute path:

```c
#include <unistd.h>
char cwd[PATH_MAX];
if (getcwd(cwd, sizeof cwd) == NULL) { perror("getcwd"); }
// cwd is now e.g. "/home/jemanuel/jek_utils/jek_find"
```

Absolute paths always begin at the root `/`. `getcwd` never returns a relative path, so this is your anchor to the system root.

**`realpath`** — turn any path (relative, with symlinks, with `.` / `..`) into the one canonical absolute path from root, with all links resolved:

```c
#include <stdlib.h>
char resolved[PATH_MAX];
if (realpath("../jek_ls/./x", resolved) == NULL) { perror("realpath"); }
// resolved is the full "/..." path with . .. and symlinks collapsed.
```

This is how you'd turn jekfind's `start_dir` into a stable absolute base, e.g. to print absolute results, or to check that two paths refer to the same place.

**`dirname` / `basename`** — split a path into "parent directory" and "final name":

```c
#include <libgen.h>
// Both may modify their argument and may return a pointer into it or into
// static storage. Always operate on a COPY.
char tmp1[] = "/home/jemanuel/file.c";
char tmp2[] = "/home/jemanuel/file.c";
char *dir  = dirname(tmp1);   // "/home/jemanuel"
char *base = basename(tmp2);  // "file.c"
```

Note: in the walk you already have the basename for free as `e->d_name`, so you rarely need `basename` during traversal — it's more useful for parsing a path you were *handed*.

## 4. man pages to read next

| Command | Covers |
|---------|--------|
| `man 3 opendir` / `man 3 readdir` / `man 3 closedir` | Directory streams and `struct dirent` |
| `man 2 stat` | `struct stat`, the `S_IS*` macros |
| `man 3 scandir` | `readdir` + sorting + filtering in one call, if you want ordered output |
| `man 3 getcwd` | Current working directory |
| `man 3 realpath` | Canonical absolute paths |
| `man 3 basename` | Covers `dirname` too, and the copy-your-arg warning |
