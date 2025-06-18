#!/bin/bash
set -e

OUTPUT_DIR="modules"
mkdir -p $OUTPUT_DIR
gcc -Wall -Werror -Isrc -shared -o $OUTPUT_DIR/sample-c.dylib src/*.c 