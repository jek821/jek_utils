# Variables — defined once here, referenced below as $(NAME)
# CC: the compiler to use
CC      = gcc
# CFLAGS: warning flags — catch common mistakes at compile time
CFLAGS  = -Wall -Wextra
# TARGET: name of the output binary
TARGET  = jekcat
# SRC: source file(s) to compile
SRC     = jekcat.c
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
	@echo "If 'jekcat' is not found, add $(BINDIR) to your PATH."

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
