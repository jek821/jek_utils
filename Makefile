# List of all tools — add a new directory here when you add a new tool
TOOLS = jek_cat jek_wc jek_ls

# Default target — builds all tools without installing
all:
	for tool in $(TOOLS); do $(MAKE) -C $$tool; done

# Installs all tools to ~/.local/bin
install:
	for tool in $(TOOLS); do $(MAKE) -C $$tool install; done

# Removes all tools from ~/.local/bin
uninstall:
	for tool in $(TOOLS); do $(MAKE) -C $$tool uninstall; done

# Removes compiled binaries from all project folders
clean:
	for tool in $(TOOLS); do $(MAKE) -C $$tool clean; done

test:
	for tool in $(TOOLS); do $(MAKE) -C $$tool test; done

.PHONY: all install uninstall clean test
