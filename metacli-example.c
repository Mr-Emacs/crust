#include <stdint.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define METACLI_IMPLEMENTATION
#include "metacli.h"

typedef struct {
    uint8_t *data;
    size_t size;
} string;

#define STRLIT(buf) (char *)(buf)->data
#define SVFMT "%.*s"
#define SVARGS(buf) (int)buf.size, STRLIT(buf)

#define reset(msg, size) memset((msg), 0, (size))
#define BREAK_POINT() asm("int3")
#define append_null(str) ((char *)(str)->data)[(str)->size] = '\0';

string *read_file(const char *file);

COMMAND("greet", "Greet someone  [-- name...]", greet)
int greet(Args *args) {
    if (args->count < 2) {
        describe(greet);
        return EXIT_FAILURE;
    }
    char buf[511] = {0};
    int  n = 0;
    for (int i = 0; i < args->count; ++i) {
        TokType t = args->tokens[i].type;
        if (t == TOK_SEP || t == TOK_FLAG || t == TOK_OPTION) continue;
        if (n++) strcat(buf, " ");
        strcat(buf, args->tokens[i].raw);
    }
    printf("Hello, %s!\n", n ? buf : "stranger");
    return EXIT_SUCCESS;
}

COMMAND("echo", "Echo tokens with their types", cmd_echo)
int cmd_echo(Args *args) {
    if (args->count < 2) {
        describe(greet);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < args->count; ++i) {
        Token *t = &args->tokens[i];
        switch (t->type) {
            case TOK_STRING: printf("[STRING] %s\n",               t->raw);                           break;
            case TOK_INT:    printf("[INT]    %s  -> %ld\n",       t->raw, t->as_int);                break;
            case TOK_FLOAT:  printf("[FLOAT]  %s  -> %g\n",        t->raw, t->as_float);              break;
            case TOK_FLAG:   printf("[FLAG]   %s\n",               t->raw);                           break;
            case TOK_OPTION: printf("[OPTION] %s  -> %s='%s'\n",   t->raw, t->as_opt.key, t->as_opt.val); break;
            case TOK_SEP:    printf("[SEP]    --\n");                                                  break;
        }
    }
    return EXIT_SUCCESS;
}

COMMAND("add", "Sum all numeric arguments", cmd_add)
int cmd_add(Args *args) {
    if (args->count < 2) {
        describe(cmd_add);
        return EXIT_FAILURE;
    }
    int nf = arg_count_type(args, TOK_FLOAT);
    int ni = arg_count_type(args, TOK_INT);
    if (ni + nf < 2) { fprintf(stderr, "usage: add <a> <b> [more...]\n"); return EXIT_FAILURE; }
    double sum = 0;
    for (int i = 0; i < ni; ++i) sum += (double)arg_int(args, i);
    for (int i = 0; i < nf; ++i) sum += arg_float(args, i);
    printf("sum = %g\n", sum);
    return EXIT_SUCCESS;
}

COMMAND("upper", "Uppercase all string arguments", cmd_upper)
int cmd_upper(Args *args) {
    if (args->count < 1) {
        describe(cmd_upper);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < arg_count_type(args, TOK_STRING); ++i) {
        const char *s = arg_string(args, i);
        for (int j = 0; s[j]; ++j) putchar(toupper((unsigned char)s[j]));
        putchar('\n');
    }
    return EXIT_SUCCESS;
}

COMMAND("sub", "Subtract numbers <a> <b> [more...]", sub)
int sub(Args *args) {
    if (args->count < 2) {
        describe(sub);
        return EXIT_FAILURE;
    }
    int ni = arg_count_type(args, TOK_INT);
    int nf = arg_count_type(args, TOK_FLOAT);

    double result = ni > 0 ? (double)arg_int(args, 0) : arg_float(args, 0);
    for (int i = 1; i < ni; i++) result -= (double)arg_int(args, i);
    for (int i = (ni == 0 ? 1 : 0); i < nf; i++) result -= arg_float(args, i);

    printf("sub = %g\n", result);
    return EXIT_SUCCESS;
}

COMMAND("file", "File processor prints multiple files [ <file1> <file2> ...]", file)
int file(Args *args) {
    if (args->count <= 0) {
        describe(file);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < args->count; ++i) {
        const char *path = arg_string(args, i);
        string *processor = read_file(path);
        printf("Read := %s", (char *)processor->data);
    }
    return EXIT_SUCCESS;
}

string *create_str(size_t size) {
  string *str = malloc(sizeof(*str));
  str->size = size;
  str->data = malloc(sizeof(*str->data) * str->size);
  return str;
}

// Just a read file function
string *read_file(const char *file) {
  FILE *f = fopen(file, "r");

  if (!f) {
    printf("Could not open file %s\n", file);
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  string *str = create_str(size);
  str->size = fread(str->data, 1, size, f);
  append_null(str);

  fclose(f);
  return str;
}

int main(int argc, char **argv) {
    CLI_RUN(argc, argv);
}
