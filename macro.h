#ifndef MACRO_H
#define MACRO_H

#define EMIT_FIELD(type, name) type name;
#define EMIT_ARRAY(type, name, size) type name[size];

#define EMIT_DESC(T, type, name) \
    FMT_FIELD_DESC(T, #type, name),

#define EMIT_DESC_ARRAY(T, type, name, size) \
    FMT_FIELD_DESC(T, #type "[" #size "]", name),

#define STRUCT(name, FIELDS) \
typedef struct name { \
    FIELDS(EMIT_FIELD, EMIT_ARRAY) \
} name; \
\
FMT_STRUCT(name, \
    FIELDS(EMIT_DESC_##name, EMIT_DESC_ARRAY_##name) \
)

#define EMIT_DESC_rect(type, name) EMIT_DESC(rect, type, name)
#define EMIT_DESC_ARRAY_rect(type, name, size) EMIT_DESC_ARRAY(rect, type, name, size)

#define EMIT_DESC_vec2f(type, name) EMIT_DESC(vec2f, type, name)
#define EMIT_DESC_ARRAY_vec2f(type, name, size) EMIT_DESC_ARRAY(vec2f, type, name, size)

#define EMIT_DESC_vec3f(type, name) EMIT_DESC(vec3f, type, name)
#define EMIT_DESC_ARRAY_vec3f(type, name, size) EMIT_DESC_ARRAY(vec3f, type, name, size)

#define EMIT_DESC_player(type, name) EMIT_DESC(player, type, name)
#define EMIT_DESC_ARRAY_player(type, name, size) EMIT_DESC_ARRAY(player, type, name, size)

#endif
