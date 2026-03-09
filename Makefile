# Compiler

ifeq ($(OS), Windows_NT)
    ifneq ($(shell where clang 2>nul), )
        CC     = clang
        CFLAGS = -Wall -Wextra -std=c11 -O2 -Wno-deprecated-declarations
    else ifneq ($(shell where cl 2>nul), )
        CC     = cl
        CFLAGS = /W4 /O2 /std:c11
    else
        $(error No supported compiler found on Windows. Install clang or MSVC.)
    endif
else
    ifneq ($(shell which gcc 2>/dev/null), )
        CC     = gcc
        CFLAGS = -Wall -Wextra -std=c11 -O2
    else ifneq ($(shell which clang 2>/dev/null), )
        CC     = clang
        CFLAGS = -Wall -Wextra -std=c11 -O2
    else
        $(error No supported compiler found. Install gcc or clang.)
    endif
endif


# Compile
all: metacli-example.exe fmt-example.exe
metacli-example.exe: metacli-example.c
	$(CC) $(CFLAGS) metacli-example.c -o metacli-example.exe

fmt-example.exe: fmt-example.c
	$(CC) $(CFLAGS) fmt-example.c -o fmt-example.exe
