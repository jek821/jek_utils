# Variables — defined once here, referenced below as $(NAME)
# CC: the compiler to use
CC      = gcc
# CFLAGS: strict compile-time checks (see Docs/strict_compile.txt).
#   -std=c11 -Wpedantic       strict ISO C11, no silent extensions
#   -D_POSIX_C_SOURCE=200809L exposes the POSIX API (open/read/getopt/...)
#                             that strict ISO mode would otherwise hide
#   -Wall -Wextra             the common + extra warning sets
#   -Wshadow                  warn on shadowed variables
#   -Wstrict-prototypes -Wmissing-prototypes   require real prototypes
#   -Wconversion              warn on implicit type conversions
#   -Werror                   treat every warning as a build failure
CFLAGS  = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic \
          -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Werror
# TARGET: name of the output binary
TARGET  = jekls
# SRC: source file(s) to compile
SRC     = jekls.c
# BINDIR: install destination; $(HOME) expands to your home dir
BINDIR  = $(HOME)/.local/bin

# Default target — runs when you type `make` with no arguments.
# Depends on $(SRC), so if the source hasn't changed since last compile, make skips this.
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Install target — runs when you type `make install`.
# Depends on $(TARGET), so make builds the binary first if needed.
install: $(TARGET)
	mkdir -p $(BINDIR)
	cp $(TARGET) $(BINDIR)/$(TARGET)
	@echo "Installed to $(BINDIR)/$(TARGET)"
	@echo "If 'jekls' is not found, add $(BINDIR) to your PATH."

# Uninstall target — removes the binary from the install location.
uninstall:
	rm -f $(BINDIR)/$(TARGET)

# Clean target — removes the compiled binary from this project folder.
clean:
	rm -f $(TARGET)

test: $(TARGET)
	bash test.sh

# Tell make that these are task names, not files.
# Without this, if a file named "install", "uninstall", or "clean" existed here,
# make would think the task was already done and skip it.
.PHONY: install uninstall clean test
