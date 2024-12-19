#!/bin/bash

TEST_DIR="tests"
EXECUTABLE="./riscv_interpreter"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: $EXECUTABLE not found. Compile it first."
    exit 1
fi

for INPUT_FILE in $TEST_DIR/*_input.txt; do
    BASE_NAME=$(basename "$INPUT_FILE" _input.txt)
    EXPECTED_FILE="$TEST_DIR/${BASE_NAME}_expected.txt"
    ACTUAL_FILE="$TEST_DIR/${BASE_NAME}_actual.txt"

    $EXECUTABLE < "$INPUT_FILE" 2> "$ACTUAL_FILE"

    if diff -u "$ACTUAL_FILE" "$EXPECTED_FILE" > /dev/null; then
        echo "Test $BASE_NAME: PASSED"
    else
        echo "Test $BASE_NAME: FAILED"
        echo "Differences:"
        diff -u "$ACTUAL_FILE" "$EXPECTED_FILE"
    fi
done
