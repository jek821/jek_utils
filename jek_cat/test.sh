#!/usr/bin/env bash
# Run from inside jek_cat/: bash test.sh

TOOL=./jekcat
PASS=0
FAIL=0

# --- helper -----------------------------------------------------------------

# check "description" "actual output" "expected output"
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

# --- setup ------------------------------------------------------------------

# mktemp creates a temp file with a unique name so tests never collide
BASIC=$(mktemp)
EMPTY=$(mktemp)
LARGE=$(mktemp)
NO_NEWLINE=$(mktemp)

# printf writes test data to temp files
printf "hello world\n"    > "$BASIC"
printf ""                 > "$EMPTY"
printf 'a%.0s' {1..9000} > "$LARGE"    # 9000 a's — crosses the 4096-byte buffer
printf "no newline"       > "$NO_NEWLINE"

# --- tests ------------------------------------------------------------------

# $(...) runs a command and captures its stdout as a string
check "basic file"  "$($TOOL "$BASIC")"       "hello world"
check "empty file"  "$($TOOL "$EMPTY")"       ""
check "no newline"  "$($TOOL "$NO_NEWLINE")"  "no newline"
check "large file"  "$($TOOL "$LARGE")"       "$(cat "$LARGE")"

# after running a command, $? holds its exit code (0 = success, 1 = failure)
$TOOL "$BASIC" > /dev/null
check "valid file exits 0" "$?" "0"

# 2>/dev/null silences stderr so error messages don't clutter test output
$TOOL /tmp/does_not_exist 2>/dev/null
check "missing file exits 1" "$?" "1"

$TOOL 2>/dev/null
check "no args exits 1" "$?" "1"

$TOOL "$BASIC" "$EMPTY" 2>/dev/null
check "too many args exits 1" "$?" "1"

# --- teardown ---------------------------------------------------------------

rm "$BASIC" "$EMPTY" "$LARGE" "$NO_NEWLINE"

# --- results ----------------------------------------------------------------

echo ""
echo "$PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
