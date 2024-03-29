TARGET ?= libexaminer.so

BUILD_DIR ?= ./build

CFLAGS ?= -Wall -Werror -Wshadow -Wconversion
TYPE ?= -O2

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): src/examiner.c src/examiner.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(TYPE) $(CFLAGS) -fpic -shared src/examiner.c -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/test: build/$(TARGET) test/main.c test/math_test.c test/each_test.c test/boolean_test.c test/str_test.c test/mem_test.c test/pending_test.c
	$(CC) $(TYPE) $(CFLAGS) \
		test/main.c \
		test/math_test.c \
		test/each_test.c \
		test/boolean_test.c \
		test/str_test.c \
		test/mem_test.c \
		test/pending_test.c \
		-o build/test -I./src -L./build -lexaminer

.PHONY: format test clean clangdhappy install uninstall
format:
	clang-format --style=file --dry-run -Werror src/examiner.c src/examiner.h

test: $(BUILD_DIR)/test
	LD_LIBRARY_PATH=${PWD}/build:${LD_LIBRARY_PATH} ./build/test

clean:
	rm -rf $(BUILD_DIR)

clangdhappy:
	compiledb make

install:
	-install -m 755 -d $(DESTDIR)$(PREFIX)/lib/
	-install -m 644 build/libexaminer.so $(DESTDIR)$(PREFIX)/lib/
	-install -m 755 -d $(DESTDIR)$(PREFIX)/include/
	-install -m 644 src/examiner.h $(DESTDIR)$(PREFIX)/include/
	-install -m 755 -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	-install -m 644 libexaminer.pc.in $(DESTDIR)$(PREFIX)/lib/pkgconfig/libexaminer.pc
	-sed -e "s:^prefix=.*:prefix=$(PREFIX):" -e "s:@@VER@@:$(VERSION):" < libexaminer.pc.in > $(DESTDIR)$(PREFIX)/lib/pkgconfig/libexaminer.pc

uninstall:
	-rm $(DESTDIR)$(PREFIX)/lib/libexaminer.so
	-rm $(DESTDIR)$(PREFIX)/include/examiner.h
