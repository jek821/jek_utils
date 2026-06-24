#!/usr/bin/env bash
# Run from inside jek_ls/: bash test.sh

TOOL=./jekls
PASS=0
FAIL=0

# --- helper -----------------------------------------------------------------

check() {
    local desc="$1" actual="$2" expected="$3"
    if [ "$actual" = "$expected" ]; then
        echo "PASS: $desc"
        ((PASS++))
    else
        echo "FAIL: $desc"
        echo "  expected: $expected"
        echo "  actual:   $actual"
        ((FAIL++))
    fi
}

# strip_ansi: remove ANSI escape codes so color doesn't break string matching
strip_ansi() {
    sed 's/\x1b\[[0-9;]*m//g'
}

# --- setup ------------------------------------------------------------------

TMPDIR=$(mktemp -d)
touch "$TMPDIR/file1.txt"
touch "$TMPDIR/file2.txt"
touch "$TMPDIR/.hidden"
mkdir "$TMPDIR/subdir"

# --- listing tests ----------------------------------------------------------

OUT=$($TOOL "$TMPDIR" 2>/dev/null | strip_ansi)

check "lists regular files"  "$(echo "$OUT" | grep -c 'file1.txt')" "1"
check "lists subdirectory"   "$(echo "$OUT" | grep -c 'subdir')"    "1"
check "hides dotfiles by default" "$(echo "$OUT" | grep -c '\.hidden')" "0"

# . and .. should never appear in default output
check "no . in default output"  "$(echo "$OUT" | grep -cx '\.')"   "0"
check "no .. in default output" "$(echo "$OUT" | grep -cx '\.\.')" "0"

# -a flag: hidden files and . / .. should appear
OUT_A=$($TOOL -a "$TMPDIR" 2>/dev/null | strip_ansi)
check "-a shows dotfiles"   "$(echo "$OUT_A" | grep -c '\.hidden')" "1"

# --- exit-code tests --------------------------------------------------------

$TOOL "$TMPDIR" > /dev/null 2>&1
check "valid directory exits 0" "$?" "0"

$TOOL /tmp/does_not_exist_jekls > /dev/null 2>&1
check "missing directory exits 1" "$?" "1"

$TOOL > /dev/null 2>&1
check "no args (current dir) exits 0" "$?" "0"

timeout 5 $TOOL -z "$TMPDIR" > /dev/null 2>&1
check "unknown flag exits 1" "$?" "1"

# --- teardown ---------------------------------------------------------------

rm -rf "$TMPDIR"

# --- results ----------------------------------------------------------------

echo ""
echo "$PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
