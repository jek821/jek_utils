# List of all tools — add a new directory here when you add a new tool
TOOLS = jek_cat jek_wc jek_ls

# Default target — builds all tools without installing
all:
	for tool in $(TOOLS); do $(MAKE) -C $$tool || exit 1; done

# Installs all tools to ~/.local/bin
install:
	for tool in $(TOOLS); do $(MAKE) -C $$tool install || exit 1; done

# Removes all tools from ~/.local/bin
uninstall:
	for tool in $(TOOLS); do $(MAKE) -C $$tool uninstall || exit 1; done

# Removes compiled binaries from all project folders
clean:
	for tool in $(TOOLS); do $(MAKE) -C $$tool clean || exit 1; done

# Runs each tool's test suite. Tools without a `test` target are skipped
# (with a note) instead of aborting the whole run.
test:
	for tool in $(TOOLS); do \
		if $(MAKE) -C $$tool -n test >/dev/null 2>&1; then \
			$(MAKE) -C $$tool test; \
		else \
			echo "Skipping $$tool (no test target)"; \
		fi; \
	done

.PHONY: all install uninstall clean test
