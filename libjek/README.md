# libjek

The shared core the tools and capstones are built on. This is where the
project stops being a pile of standalone tools and becomes a system.

## The three layers

```
libjek/            the core: non-trivial logic needed by more than one consumer.
                   Tested in isolation. This is where "real, not toy" is won.

jek_find/ jek_sum/ thin CLIs over libjek. They stay as learning artifacts AND
jek_ls/  ...       double as the tests that exercise each library module.

jeksync  jekvpn    the capstones. They compose libjek, not the tools.
```

Toy-vs-real is decided almost entirely by whether load-bearing logic (atomic
writes, streaming hashes, the wire codec, the crypto session, the thread pool)
lives behind a tested API here, or is copy-pasted into a dozen happy-path tool
`main()`s where only the demo case ever runs.

## What belongs here

A module earns a place in libjek when it is **both**:

- non-trivial (a tree walk, a buffered/atomic I/O layer, a streaming hash, a
  protocol codec, a crypto session, a thread pool), **and**
- genuinely needed by a second real consumer.

Trivial helpers (jekcat's copy loop) and per-tool concerns (a tool's own
argument parsing) do **not** belong here. Duplication is cheaper than the wrong
abstraction.

## How things get here: organic extraction, not up-front design

Do not design libjek's APIs ahead of their callers. The right abstraction is
*discovered* by extracting from two real call sites, never guessed at before
they exist. The rule:

  write it in the first tool. write it in the second tool. NOW extract it --
  the two worked examples tell you the exact shape the API should be.

Extraction is a natural consequence of noticing overlap while building, not a
scheduled phase. libjek is a destination, not a milestone.

First likely extraction: `jek_walk` (file-tree traversal), which jekfind and
jeksum both already need.

## Module convention

Same naming as the tools, minus the binary:

  libjek/jek_<thing>.c   implementation
  libjek/jek_<thing>.h   public API (with an include guard -- headers here get
                         included by each other and by consumers)

`libjek/` builds to a static archive `libjek.a`. A tool or capstone links it by
adding the archive to its link line and `-I../libjek` for the headers. The
Makefile wiring lands with the first real extraction, not before -- an empty
archive is just dead scaffolding.

## Likely modules (not a mandate -- these appear as the code asks for them)

  jek_walk    recursive file-tree traversal        (jekfind, jeksum, jeksync)
  jek_io      buffered read/write; atomic write via temp + fsync + rename
  jek_hash    streaming BLAKE2b over a file descriptor (libsodium)
  jek_proto   length-prefixed, typed binary message codec
  jek_crypto  the Noise session/handshake           (jeksync transport + jekvpn)
  jek_pool    the pthreads work queue                (already sketched in jeksum.h)
