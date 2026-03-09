#define _CRT_SECURE_NO_WARNINGS

#define FMT_CUSTOM_TYPES \
rect: _fmt_mk_rect, \
vec2f: _fmt_mk_vec2f, \
vec3f: _fmt_mk_vec3f

#define FMT_IMPLEMENTATION
#define FMT_STRIP_PREFIX
#include "fmt.h"
#include "macro.h"

#define vec2f_FIELDS(F, A) \
    F(i32, h)   \
    F(i32, z)
STRUCT(vec2f, vec2f_FIELDS)

#define vec3f_FIELDS(F, A) \
    F(f32, x)   \
    F(f32, y)   \
    F(f32, z)
STRUCT(vec3f, vec3f_FIELDS)

#define RECT_FIELDS(F, A) \
    F(i32, w) \
    A(i32, v, 2) \
    F(i32, h)   \
    A(i32, z, 3)
STRUCT(rect, RECT_FIELDS)

int main(void) {
    rect rec = { .h = 10, .z = {20, 20, 40}};
    println("%", rec);

    vec2f v = { .h = 10.0f, .z = 20.0f};
    println("%", v);

    vec3f v3 = {.x = 10, .y = 20, .z = 40};
    println("%", v3);

    println("% % %", v3.x, v3.y, v3.z);
}
