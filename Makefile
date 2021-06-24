TARGET ?= libexaminer.so

BUILD_DIR ?= ./build

CFLAGS ?= -Wall -Werror -fpic
TYPE ?= -Og -ggdb3

all: $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/test

$(BUILD_DIR)/$(TARGET): src/examiner.c src/examiner.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(TYPE) $(CFLAGS) -shared src/examiner.c -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/test: build/$(TARGET) test/examiner_test.c
	$(CC) $(TYPE) $(CFLAGS) test/examiner_test.c -o build/test -I./src -L./build -lexaminer

.PHONY: format test clean clangdhappy
format:
	clang-format --style=file --dry-run -Werror src/examiner.c src/examiner.h

test: build/test
	LD_LIBRARY_PATH=${PWD}/build:${LD_LIBRARY_PATH} ./build/test

clean:
	rm -rf $(BUILD_DIR)

clangdhappy:
	compiledb make
