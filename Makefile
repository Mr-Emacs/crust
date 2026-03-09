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

BUILD   := build/
NAMES    = metacli-example fmt-example
TARGETS  = $(addprefix $(BUILD), $(NAMES))

all: $(BUILD) $(TARGETS)

$(BUILD):
ifeq ($(OS), Windows_NT)
	if not exist build md build
else
	mkdir -p $(BUILD)
endif

$(BUILD)%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
ifeq ($(OS), Windows_NT)
	if exist build rd /s /q build
else
	rm -rf $(BUILD)
endif
