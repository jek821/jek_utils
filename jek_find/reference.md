# jekfind syscall / libc reference

How to walk a file tree from a starting directory, and how to work out where you are relative to the system root (`/`). Covers `nftw()`, the manual `opendir`/`readdir`/`stat` recursion it wraps, and the path helpers (`getcwd`, `realpath`, `dirname`/`basename`).

## 1. `nftw()` — the easy button for tree walking

```c
#include <ftw.h>

int nftw(const char *dirpath,
         int (*fn)(const char *fpath, const struct stat *sb,
                   int typeflag, struct FTW *ftwbuf),
         int nopenfd,
         int flags);
```

`nftw` = "new file tree walk". You hand it a starting directory and a callback. It recurses the whole subtree for you and calls your callback once per entry (files, directories, symlinks — everything). This is exactly the "recursive directory traversal" jekfind is built around, so `nftw` is the shortcut version of what section 2 does by hand.

**Arguments**

| Argument | Meaning |
|----------|---------|
| `dirpath` | Where to start (e.g. `"."` or `"/home/jemanuel"`). |
| `fn` | Your callback, called for every entry found. |
| `nopenfd` | Max number of directory file descriptors `nftw` may hold open at once. It uses one per level of depth. A sane value is 15–20; if the tree is deeper, `nftw` closes/reopens dirs as needed (slower but still correct). Not a limit on tree depth. |
| `flags` | Bitwise OR of the `FTW_*` option flags below. |

**Return:** `0` if the whole walk finished. If your callback ever returns non-zero, `nftw` stops immediately and returns that same value (this is how you do an early exit / "found it, stop"). `-1` on a setup error (`errno` set).

**The callback arguments**

| Argument | Meaning |
|----------|---------|
| `fpath` | Full path to the current entry, built from `dirpath`. If you started at `"."`, paths look like `"./src/main.c"`. If you started at `"/home"`, they look like `"/home/jemanuel/file"`. |
| `sb` | Pointer to the stat struct for this entry (size, mode, mtime, …). Same data as calling `stat()` yourself — see section 3. |
| `typeflag` | What kind of entry this is (see next table). |
| `ftwbuf` | `struct FTW` with two useful fields: `ftwbuf->level` = depth below the start dir (start = 0); `ftwbuf->base` = offset into `fpath` where the basename starts, i.e. `fpath + ftwbuf->base` is just the filename without the leading directories. |

**`typeflag` values (what the current entry is)**

| Value | Meaning |
|-------|---------|
| `FTW_F` | A regular file. |
| `FTW_D` | A directory, reported BEFORE its contents (pre-order). |
| `FTW_DP` | A directory, reported AFTER its contents. Only happens if you pass `FTW_DEPTH` in flags. Use this when you need to process a directory's children before the directory itself (e.g. deleting a tree, or summing child sizes). |
| `FTW_SL` | A symbolic link (reported when you pass `FTW_PHYS`). |
| `FTW_SLN` | A dangling symlink (points at something that doesn't exist). |
| `FTW_DNR` | A directory that can't be read (permissions). Its contents are skipped. |
| `FTW_NS` | `stat()` failed on this entry, so `sb` is NOT valid — don't touch it. Usually a permissions problem on the parent dir. |

**`flags` (bitwise OR them together)**

| Flag | Meaning |
|------|---------|
| `FTW_PHYS` | Physical walk: do NOT follow symlinks, and report them as `FTW_SL`. Almost always what you want for a find-like tool, so you don't loop forever on a link that points back up the tree. |
| `FTW_MOUNT` | Stay on one filesystem; don't cross mount points. |
| `FTW_DEPTH` | Post-order traversal: report a directory (`FTW_DP`) only after all its children. Without this you get pre-order (`FTW_D` first). |

**Minimal skeleton**

```c
static int cb(const char *fpath, const struct stat *sb,
              int typeflag, struct FTW *ftwbuf) {
    (void)sb;
    const char *name = fpath + ftwbuf->base;   // basename only
    if (typeflag == FTW_D)      printf("[dir]  %s\n", fpath);
    else if (typeflag == FTW_F) printf("[file] %s (name=%s)\n", fpath, name);
    return 0;   // return non-zero to stop the walk early
}

int main(void) {
    // start ".", up to 20 open fds, don't follow symlinks
    if (nftw(".", cb, 20, FTW_PHYS) == -1) {
        perror("nftw");
        return 1;
    }
    return 0;
}
```

For jekfind this maps cleanly: `-type f` → only act on `FTW_F`, `-type d` → only `FTW_D`. `-name`/`-contains` → compare (`fpath + ftwbuf->base`) against the target with `strcmp` / `strstr` / `strcasecmp`.

> **Caveat:** `nftw` has no way to pass your own context pointer into the callback (no `void*` userdata arg). So the `Flags` struct in `jekfind.h` has to be reached via a file-scope (static global) variable.

## 2. Doing it by hand: `opendir` / `readdir` / `stat` + recursion

`nftw` is just a wrapper over this. Worth writing once yourself so you know what it's doing — and jek_ls already uses `opendir`/`readdir`/`stat`, so this is the same primitives plus recursion.

```c
void walk(const char *path) {
    DIR *d = opendir(path);
    if (!d) { perror(path); return; }

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

        printf("%s\n", child);

        if (S_ISDIR(sb.st_mode))
            walk(child);          // recurse into subdirectory
    }
    closedir(d);
}
```

**Key points that bite people**

- `readdir` returns `.` and `..` — you MUST skip them.
- `struct dirent` has `d_type` (`DT_DIR`, `DT_REG`, `DT_LNK`, …) which can save a `stat()` call, BUT some filesystems return `DT_UNKNOWN`, so you still need a `stat()` fallback.
- Use `lstat` (not `stat`) when recursing so a symlinked directory doesn't send you into a loop — this is the manual equivalent of `nftw`'s `FTW_PHYS`.
- `snprintf` (not `strcpy`/`strcat`) to build paths, so you can't overflow.

## 3. `struct stat` — what you learn about each entry

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
| `sb.st_ino` | Inode number. (`st_dev`, `st_ino`) together uniquely identify a file — how you'd detect hardlinks or a link that loops back. |

In an `nftw` callback you already get this as `*sb`, so you rarely call `stat` yourself during a walk.

## 4. "Where am I relative to `/`" — absolute paths and the root

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

## 5. man pages to read next

| Command | Covers |
|---------|--------|
| `man 3 nftw` | Documents `struct FTW` and all the flags |
| `man 3 opendir` / `man 3 readdir` / `man 3 closedir` | Directory streams |
| `man 2 stat` | `struct stat`, the `S_IS*` macros |
| `man 3 getcwd` | Current working directory |
| `man 3 realpath` | Canonical absolute paths |
| `man 3 basename` | Covers `dirname` too, and the copy-your-arg warning |
