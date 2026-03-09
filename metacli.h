#ifndef METACLI_H
#define METACLI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOK_STRING,
    TOK_INT,
    TOK_FLOAT,
    TOK_FLAG,
    TOK_OPTION,
    TOK_SEP,
} TokType;

typedef struct {
    TokType     type;
    const char *raw;
    union {
        long   as_int;
        double as_float;
        struct { char key[64]; char val[256]; } as_opt;
    };
} Token;

typedef struct {
    Token *tokens;
    int    count;
} Args;

typedef int (*CommandFn)(Args *args);

typedef struct {
    const char *name;
    const char *description;
    CommandFn   run;
} Command;

#define METACLI_MAX_COMMANDS 64
#define METACLI_MAX_TOKENS   128

extern Command _metacli_registry[METACLI_MAX_COMMANDS];
extern int     _metacli_registry_len;

void _metacli_register(const char *name, const char *desc, CommandFn fn);
int  _metacli_run(int argc, char **argv);
static inline void _metacli_describe(CommandFn fn);

#if defined(__GNUC__) || defined(__clang__)
#  define COMMAND(name, desc, fn)                                   \
     int fn(Args *args);                                            \
     __attribute__((constructor))                                   \
     static void _metacli_reg_##fn(void) {                          \
         _metacli_register(name, desc, fn);                         \
     }
#elif defined(_MSC_VER)
#  error "MSVC is not support and never will be"
#else
#  error "COMMAND() auto-registration requires GCC or Clang"
#endif

#define CLI_RUN(argc, argv) return _metacli_run((argc), (argv))
#define describe(fn) _metacli_describe(fn)

static inline Token *_args_nth_typed(Args *a, TokType t, int n) {
    int seen = 0;
    for (int i = 0; i < a->count; ++i)
        if (a->tokens[i].type == t && seen++ == n) return &a->tokens[i];
    return NULL;
}

static inline const char *arg_string (Args *a, int n) {
    Token *t = _args_nth_typed(a, TOK_STRING, n);
    return t ? t->raw : NULL;
}
static inline long arg_int(Args *a, int n) {
    Token *t = _args_nth_typed(a, TOK_INT, n);
    return t ? t->as_int : 0;
}
static inline double arg_float(Args *a, int n) {
    Token *t = _args_nth_typed(a, TOK_FLOAT, n);
    return t ? t->as_float : 0.0;
}
static inline int arg_flag(Args *a, const char *flag) {
    for (int i = 0; i < a->count; ++i)
        if (a->tokens[i].type == TOK_FLAG && strcmp(a->tokens[i].raw + 1, flag) == 0)
            return 1;
    return 0;
}
static inline const char *arg_option(Args *a, const char *key) {
    for (int i = 0; i < a->count; ++i)
        if (a->tokens[i].type == TOK_OPTION && strcmp(a->tokens[i].as_opt.key, key) == 0)
            return a->tokens[i].as_opt.val[0] ? a->tokens[i].as_opt.val : "1";
    return NULL;
}
static inline int arg_count_type(Args *a, TokType t) {
    int n = 0;
    for (int i = 0; i < a->count; ++i) if (a->tokens[i].type == t) ++n;
    return n;
}

#endif /* METACLI_H */


#ifdef METACLI_IMPLEMENTATION

Command _metacli_registry[METACLI_MAX_COMMANDS];
int     _metacli_registry_len = 0;

void _metacli_register(const char *name, const char *desc, CommandFn fn) {
    if (_metacli_registry_len >= METACLI_MAX_COMMANDS) {
        fprintf(stderr, "metacli: registry full\n"); return;
    }
    _metacli_registry[_metacli_registry_len++] = (Command){ name, desc, fn };
}

static void _lex_token(const char *raw, Token *tok) {
    tok->raw = raw;
    if (strcmp(raw, "--") == 0) { tok->type = TOK_SEP; return; }
    if (raw[0] == '-' && raw[1] == '-') {
        tok->type = TOK_OPTION;
        const char *eq = strchr(raw + 2, '=');
        if (eq) {
            size_t kl = (size_t)(eq - (raw+2)); if (kl > 63) kl = 63;
            strncpy(tok->as_opt.key, raw+2, kl); tok->as_opt.key[kl] = '\0';
            strncpy(tok->as_opt.val, eq+1, 255); tok->as_opt.val[255] = '\0';
        } else {
            strncpy(tok->as_opt.key, raw+2, 63); tok->as_opt.key[63] = '\0';
            tok->as_opt.val[0] = '\0';
        }
        return;
    }
    if (raw[0] == '-' && raw[1] && !isdigit((unsigned char)raw[1])) {
        tok->type = TOK_FLAG; return;
    }
    {
        const char *p = raw; if (*p == '-') ++p;
        const char *s = p;
        while (isdigit((unsigned char)*p)) ++p;
        if (*p == '\0' && p != s) { tok->type = TOK_INT; tok->as_int = atol(raw); return; }
    }
    {
        char *end; double d = strtod(raw, &end);
        if (*end == '\0' && end != raw) { tok->type = TOK_FLOAT; tok->as_float = d; return; }
    }
    tok->type = TOK_STRING;
}

static int _builtin_help(Args *args) {
    (void)args;
    fprintf(stderr, "Commands:\n");
    for (int i = 0; i < _metacli_registry_len; ++i)
        fprintf(stderr, "    %-16s %s\n",
                _metacli_registry[i].name,
                _metacli_registry[i].description);
    return EXIT_SUCCESS;
}

int _metacli_run(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        _builtin_help(NULL);
        return EXIT_FAILURE;
    }
    const char *cmd = argv[1];
    if (strcmp(cmd, "help") == 0) { _builtin_help(NULL); return EXIT_SUCCESS; }
    for (int i = 0; i < _metacli_registry_len; ++i) {
        if (strcmp(_metacli_registry[i].name, cmd) == 0) {
            static Token toks[METACLI_MAX_TOKENS];
            int n = 0;
            for (int j = 2; j < argc && n < METACLI_MAX_TOKENS; ++j)
                _lex_token(argv[j], &toks[n++]);
            Args a = { toks, n };
            return _metacli_registry[i].run(&a);
        }
    }
    fprintf(stderr, "ERROR: unknown command '%s'\n", cmd);
    _builtin_help(NULL);
    return EXIT_FAILURE;
}

static inline void _metacli_describe(CommandFn fn) {
    for (int i = 0; i < _metacli_registry_len; ++i)
        if (_metacli_registry[i].run == fn) {
            printf("Usage:\n");
            fprintf(stderr, "    %-16s %s\n",
                    _metacli_registry[i].name,
                    _metacli_registry[i].description);
        }
}

#endif /* METACLI_IMPLEMENTATION */
