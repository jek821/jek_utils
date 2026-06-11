# Variables — defined once here, referenced below as $(NAME)
CC      = gcc                  # the compiler to use
CFLAGS  = -Wall -Wextra        # warning flags: catch common mistakes at compile time
TARGET  = jekcat               # name of the output binary
SRC     = jekcat.c             # source file(s) to compile
BINDIR  = $(HOME)/.local/bin   # install destination; $(HOME) expands to your home dir

# Default target — runs when you type `make` with no arguments.
# Depends on $(SRC), so if the source hasn't changed since last compile, make skips this.
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Install target — runs when you type `make install`.
# Depends on $(TARGET), so make builds the binary first if needed.
install: $(TARGET)
	mkdir -p $(BINDIR)                    # create ~/.local/bin if it doesn't exist (-p means no error if it already does)
	cp $(TARGET) $(BINDIR)/$(TARGET)      # copy the binary there
	@echo "Installed to $(BINDIR)/$(TARGET)"
	@echo "If 'jekcat' is not found, add $(BINDIR) to your PATH."

# Uninstall target — removes the binary from the install location.
uninstall:
	rm -f $(BINDIR)/$(TARGET)    # -f means no error if the file doesn't exist

# Clean target — removes the compiled binary from this project folder.
clean:
	rm -f $(TARGET)

# Tell make that these are task names, not files.
# Without this, if a file named "install", "uninstall", or "clean" existed here,
# make would think the task was already done and skip it.
.PHONY: install uninstall clean
