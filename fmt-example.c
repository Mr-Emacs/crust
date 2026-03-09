#define _CRT_SECURE_NO_WARNINGS

#define FMT_CUSTOM_TYPES \
rect: _fmt_mk_rect, \
vec2f: _fmt_mk_vec2f

#define FMT_IMPLEMENTATION
#define FMT_STRIP_PREFIX
#include "fmt.h"
#include "macro.h"

#define vec2f_FIELDS(F, A) \
    F(i32, h)   \
    F(i32, z)
STRUCT(vec2f, vec2f_FIELDS)

#define RECT_FIELDS(F, A) \
    F(i32, w) \
    A(i32, v, 2) \
    F(i32, h)   \
    A(i32, z, 3)
STRUCT(rect, RECT_FIELDS)

int main(void) {
    rect r = {
        .z = {12, 34, 35},
        .v = {10.0f, 10.0f},
        .h = 50,
        .w = 50,
    };

    println("%", r);

    vec2f v = { .h = 10.0f, .z = 20.0f};
    println("%", v);
    println("% % %", 1, 3, 4);
}
