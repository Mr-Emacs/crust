#ifndef FFMT_H
#define FFMT_H

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#define FMT_BUF_SIZE 1024

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t   s32;
typedef float    f32;
typedef double   f64;
typedef bool     b32;

typedef enum { FMT_ALLOC_ERR = -1, FMT_CONV_ERR } err;

typedef enum {
    FMT_I8,FMT_I16,FMT_I32,FMT_I64,
    FMT_U8,FMT_U16,FMT_U32,FMT_U64,
    FMT_F32,FMT_F64,
    FMT_STR,
    FMT_BOOL,
    FMT_CUSTOM,
    FMT_COUNT
} fmt_types;

typedef int (*fmt_custom_fn)(char*,size_t,void*);

typedef struct{
    fmt_types type;
    union{
        i8 i8;i16 i16;i32 i32;i64 i64;
        u8 u8;u16 u16;u32 u32;u64 u64;
        f32 f32;f64 f64;
        char*s;
        _Bool b;
        struct{void*ptr;fmt_custom_fn fn;}custom;
    };
}fmt_val;

#define arr_len(arr)((int)(sizeof(arr)/sizeof(arr[0])))

static inline fmt_val _fmt_mk_i8(i8 x){return(fmt_val){FMT_I8,.i8=x};}
static inline fmt_val _fmt_mk_i16(i16 x){return(fmt_val){FMT_I16,.i16=x};}
static inline fmt_val _fmt_mk_i32(i32 x){return(fmt_val){FMT_I32,.i32=x};}
static inline fmt_val _fmt_mk_i64(i64 x){return(fmt_val){FMT_I64,.i64=x};}
static inline fmt_val _fmt_mk_u8(u8 x){return(fmt_val){FMT_U8,.u8=x};}
static inline fmt_val _fmt_mk_u16(u16 x){return(fmt_val){FMT_U16,.u16=x};}
static inline fmt_val _fmt_mk_u32(u32 x){return(fmt_val){FMT_U32,.u32=x};}
static inline fmt_val _fmt_mk_u64(u64 x){return(fmt_val){FMT_U64,.u64=x};}
static inline fmt_val _fmt_mk_f32(f32 x){return(fmt_val){FMT_F32,.f32=x};}
static inline fmt_val _fmt_mk_f64(f64 x){return(fmt_val){FMT_F64,.f64=x};}
static inline fmt_val _fmt_mk_str(char*x){return(fmt_val){FMT_STR,.s=x};}
static inline fmt_val _fmt_mk_bool(_Bool x){return(fmt_val){FMT_BOOL,.b=x};}

#ifndef FMT_CUSTOM_TYPES
#define FMT_CUSTOM_TYPES
#endif
#define _FMT_ARG(x) _Generic((x), \
int8_t:_fmt_mk_i8, \
int16_t:_fmt_mk_i16, \
int32_t:_fmt_mk_i32, \
int64_t:_fmt_mk_i64, \
uint8_t:_fmt_mk_u8, \
uint16_t:_fmt_mk_u16, \
uint32_t:_fmt_mk_u32, \
uint64_t:_fmt_mk_u64, \
float:_fmt_mk_f32, \
double:_fmt_mk_f64, \
char*:_fmt_mk_str, \
_Bool:_fmt_mk_bool, \
FMT_CUSTOM_TYPES, \
default:_fmt_mk_str)(x)

typedef struct{
    int pp;
    int indent;
    int newline;
}fmt_opt;

#define FMT_OPT_DEFAULT (fmt_opt){0,2,1}

#define _FMAP1(a) _FMT_ARG(a)
#define _FMAP2(a,b) _FMT_ARG(a),_FMT_ARG(b)
#define _FMAP3(a,b,c) _FMT_ARG(a),_FMT_ARG(b),_FMT_ARG(c)
#define _FMAP4(a,b,c,d) _FMT_ARG(a),_FMT_ARG(b),_FMT_ARG(c),_FMT_ARG(d)
#define _FMAP5(a,b,c,d,e) _FMT_ARG(a),_FMT_ARG(b),_FMT_ARG(c),_FMT_ARG(d),_FMT_ARG(e)
#define _FMAP6(a,b,c,d,e,f) _FMT_ARG(a),_FMT_ARG(b),_FMT_ARG(c),_FMT_ARG(d),_FMT_ARG(e),_FMT_ARG(f)
#define _FMAP7(a,b,c,d,e,f,g) _FMT_ARG(a),_FMT_ARG(b),_FMT_ARG(c),_FMT_ARG(d),_FMT_ARG(e),_FMT_ARG(f),_FMT_ARG(g)

#define _FMAP_N(_1,_2,_3,_4,_5,_6,_7,N,...) _FMAP##N
#define _FMAP_EACH(...) _FMAP_N(__VA_ARGS__,7,6,5,4,3,2,1)(__VA_ARGS__)

#define f_print(fmt,...) do { \
    fmt_val _vals[] = { _FMAP_EACH(__VA_ARGS__) }; \
    ffmt_impl_opt(FMT_OPT_DEFAULT, fmt, arr_len(_vals), _vals); \
} while(0)

#define f_println(fmt,...) do { \
    fmt_val _vals[] = { _FMAP_EACH(__VA_ARGS__) }; \
    ffmt_impl_opt_ln(FMT_OPT_DEFAULT, fmt, arr_len(_vals), _vals); \
} while(0)

void ffmt_impl_opt(fmt_opt,const char*,size_t,fmt_val*);
void ffmt_impl_opt_ln(fmt_opt,const char*,size_t,fmt_val*);

typedef enum{
    FT_I8,FT_I16,FT_I32,FT_I64,
    FT_U8,FT_U16,FT_U32,FT_U64,
    FT_F32,FT_F64,
    FT_STR,
    FT_BOOL,
    FT_ARRAY,
    FT_CUSTOM,
    FT_UNRESOLVED,
    FT_UNKNOWN
}field_tag;

typedef struct{
    const char*type_str;
    field_tag tag;
    field_tag elem_tag;
    int arr_count;
    size_t offset;
    char name[32];
}field_desc;

#define FMT_FIELD_DESC(T,type,fname)\
{type,FT_UNRESOLVED,FT_UNKNOWN,0,offsetof(T,fname),#fname}

static field_tag _resolve_base(const char*s){

    if(!strcmp(s,"i8")||!strcmp(s,"int8_t"))return FT_I8;
    if(!strcmp(s,"i16")||!strcmp(s,"int16_t"))return FT_I16;
    if(!strcmp(s,"i32")||!strcmp(s,"int32_t"))return FT_I32;
    if(!strcmp(s,"i64")||!strcmp(s,"int64_t"))return FT_I64;

    if(!strcmp(s,"u8")||!strcmp(s,"uint8_t"))return FT_U8;
    if(!strcmp(s,"u16")||!strcmp(s,"uint16_t"))return FT_U16;
    if(!strcmp(s,"u32")||!strcmp(s,"uint32_t"))return FT_U32;
    if(!strcmp(s,"u64")||!strcmp(s,"uint64_t")||!strcmp(s,"size_t"))return FT_U64;

    if(!strcmp(s,"f32")||!strcmp(s,"float"))return FT_F32;
    if(!strcmp(s,"f64")||!strcmp(s,"double"))return FT_F64;

    if(!strcmp(s,"bool")||!strcmp(s,"_Bool"))return FT_BOOL;

    if(!strcmp(s,"char*"))return FT_STR;

    return FT_UNKNOWN;
}

static void _resolve_field(field_desc*f){

    if(f->tag!=FT_UNRESOLVED)return;

    char buf[64];
    strncpy(buf,f->type_str,sizeof(buf));

    char*br=strchr(buf,'[');

    if(br){

        *br=0;
        int count=atoi(br+1);

        f->tag=FT_ARRAY;
        f->elem_tag=_resolve_base(buf);
        f->arr_count=count;

    }else{
        f->tag = _resolve_base(buf);
    }

}

static int _fmt_field_at(char*buf,size_t sz,void*ptr,field_tag tag){

    switch(tag){

        case FT_I8:return snprintf(buf,sz,"%d",*(i8*)ptr);
        case FT_I16:return snprintf(buf,sz,"%d",*(i16*)ptr);
        case FT_I32:return snprintf(buf,sz,"%d",*(i32*)ptr);
        case FT_I64:return snprintf(buf,sz,"%lld",(long long)*(i64*)ptr);

        case FT_U8:return snprintf(buf,sz,"%u",*(u8*)ptr);
        case FT_U16:return snprintf(buf,sz,"%u",*(u16*)ptr);
        case FT_U32:return snprintf(buf,sz,"%u",*(u32*)ptr);
        case FT_U64:return snprintf(buf,sz,"%llu",(unsigned long long)*(u64*)ptr);

        case FT_F32:return snprintf(buf,sz,"%f",(double)*(f32*)ptr);
        case FT_F64:return snprintf(buf,sz,"%f",*(f64*)ptr);

        case FT_STR:return snprintf(buf,sz,"%s",*(char**)ptr);

        case FT_BOOL:return snprintf(buf,sz,"%s",*(bool*)ptr?"true":"false");

        default:return snprintf(buf,sz,"?");
    }
}

static int _fmt_elem_size(field_tag tag){

    switch(tag){

        case FT_I8:case FT_U8:case FT_BOOL:return 1;
        case FT_I16:case FT_U16:return 2;
        case FT_I32:case FT_U32:case FT_F32:return 4;
        case FT_I64:case FT_U64:case FT_F64:return 8;
        case FT_STR:return sizeof(char*);
        default:return 0;
    }
}

static int fmt_struct_format(char*buf,size_t sz,
        const char*tname,
        void*ptr,
        field_desc*fields,int nfields){

    int w=0;

    w+=snprintf(buf+w,sz-w,"%s{",tname);

    for(int i=0;i<nfields;i++){

        _resolve_field(&fields[i]);

        if(i)w+=snprintf(buf+w,sz-w," ");

        w+=snprintf(buf+w,sz-w,"%s=",fields[i].name);

        void*fp=(char*)ptr+fields[i].offset;

        if(fields[i].tag==FT_ARRAY){

            int es=_fmt_elem_size(fields[i].elem_tag);

            w+=snprintf(buf+w,sz-w,"[");

            for(int j=0;j<fields[i].arr_count;j++){

                if(j)w+=snprintf(buf+w,sz-w,", ");

                void*ep=(char*)fp+j*es;

                w+=_fmt_field_at(buf+w,sz-w,ep,fields[i].elem_tag);

            }

            w+=snprintf(buf+w,sz-w,"]");
        }else{
            w+=_fmt_field_at(buf+w,sz-w,fp,fields[i].tag);
        }

    }

    w+=snprintf(buf+w,sz-w,"}");

    return w;
}

#define FMT_REGISTER(T,fn)\
    static inline fmt_val _fmt_mk_##T(T v){\
        static T buf;buf=v;\
        return(fmt_val){FMT_CUSTOM,.custom={&buf,fn}};}

#define FMT_STRUCT(T,...)\
    static field_desc _fmtfields_##T[]={__VA_ARGS__};\
    static int _fmt_fn_##T(char*buf,size_t sz,void*p){\
        return fmt_struct_format(buf,sz,#T,p,_fmtfields_##T,arr_len(_fmtfields_##T));}\
        FMT_REGISTER(T,_fmt_fn_##T)

#if defined(FMT_IMPLEMENTATION)

static int fmt_val_to_str(char*buf,size_t sz,fmt_val v){

    switch(v.type){

        case FMT_I32:return snprintf(buf,sz,"%d",v.i32);
        case FMT_F32:return snprintf(buf,sz,"%f",v.f32);
        case FMT_STR:return snprintf(buf,sz,"%s",v.s);
        case FMT_BOOL:return snprintf(buf,sz,"%s",v.b?"true":"false");
        case FMT_CUSTOM:return v.custom.fn(buf,sz,v.custom.ptr);

        default:return snprintf(buf,sz,"?");
    }

}

static void _ffmt_core(char *buf, size_t sz,
        const char *fmt,
        size_t count,
        fmt_val *vals,
        fmt_opt opt)
{
    (void)sz;
    int bi=0;
    int vi=0;

    int indent_level = 0;

    for(int i=0; fmt[i]; i++){

        if(fmt[i]=='%'){

            if(vi >= (int)count) continue;

            char tmp[256]={0};

            int len = fmt_val_to_str(tmp,sizeof(tmp),vals[vi++]);

            if(opt.pp){

                if(tmp[0]=='{'){
                    indent_level++;
                }

                buf[bi++]='\n';

                for(int k=0;k<indent_level*opt.indent;k++)
                    buf[bi++]=' ';
            }

            memcpy(buf+bi,tmp,len);
            bi+=len;

        }
        else{
            buf[bi++]=fmt[i];
        }
    }

    buf[bi]=0;
}

void ffmt_impl_opt(fmt_opt opt,const char*fmt,size_t count,fmt_val*vals){

    char buf[FMT_BUF_SIZE]={0};

    _ffmt_core(buf,FMT_BUF_SIZE,fmt,count,vals,opt);

    printf("%s",buf);

}

void ffmt_impl_opt_ln(fmt_opt opt,const char*fmt,size_t count,fmt_val*vals){

    char buf[FMT_BUF_SIZE]={0};

    _ffmt_core(buf,FMT_BUF_SIZE,fmt,count,vals,opt);

    printf("%s\n",buf);

}

#endif

#if defined(FMT_STRIP_PREFIX)
#define print   f_print
#define println f_println
#endif

#endif
