/* tiny_json - v1.00 - public domain json parser - https://mazluc.ch/tiny
                                no warranty implied; use at your own risk
   Do this:
      #define JSON_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the
   implementation

   // It should look like this:
   #include ...
   #define JSON_IMPLEMENTATION
   #include "tiny_json.h"

   QUICK NOTES:
      

   EXAMPLE:
      

   LICENSE:

      See end of file for license information.

   RECENT REVISION HISTORY:

      See end of file for full revision history.

   ============================    Contributors    =========================

   Structure:
      Luca Mazza
*/

#ifndef JSON_DEFINITIONS
#define JSON_DEFINITIONS

#include <cstddef>
#include <cstring>
#ifndef JSON_NO_STDLIB
#include <stdlib.h>
#endif

#ifndef JSON_NO_STDDEF
#include <stddef.h>
#endif

#ifndef JSON_NO_STRING
#include <string.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { JSON_NULL, JSON_BOOL, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT } json_type;

typedef struct json_value json_value;
typedef struct { int line, col; const char *msg; } json_error;
typedef void *(*json_malloc_fn)(size_t, void *);
typedef void  (*json_free_fn)(void *, void *);

typedef struct {
    json_malloc_fn malloc_fn;
    json_free_fn free_fn;
    void *user;
    json_error err;
} json_allocator;

json_value *json_parse(const char *src, json_allocator *a);
char *json_stringify(const json_value *v, int flags, json_allocator *a);
void json_free(json_value *v, json_allocator *a);
void json_free_string(char *s, json_allocator *a);

#ifdef __cplusplus
}
#endif

#endif

#define JSON_IMPLEMENTATION
#ifdef JSON_IMPLEMENTATION

struct json_value {
    json_type type;
    union {
        double number;
        int boolean;
        struct { char *ptr; size_t len; } string;
        struct { struct json_value **items; size_t len, cap; } array;
        struct {
            struct { char *key; struct json_value *val; } *items;
            size_t len, cap;
        } object;
    } as;
};

typedef struct {
    const char *s;
    size_t i, line, col;
    json_allocator *a;
} json_parser;

static void *json_alloc(json_allocator *a, size_t n) {
    if (a && a->malloc_fn) return a->malloc_fn(n, a->user);
    return malloc(n);
}

static void json_dealloc(json_allocator* a, void *p) {
    if (!p) return;
    if (a && a->free_fn) a->free_fn(p, a->user);
    else free(p);
}

static json_value *json_new(json_type type, json_allocator *a) {
    json_value *v = (json_value*)json_alloc(a, sizeof(*v));
    if (!v) return NULL;
    memset(v, 0, sizeof(*v));
    v->type = type;
    return v;
}

static json_value *json_fail(json_parser *p, const char *msg) {
    if (p->a) {
        p->a->err.line = (int)p->line;
        p->a->err.col = (int)p->col;
        p->a->err.msg = msg;
    }
    return NULL;
}

static void json_skip_ws(json_parser *p) {
    while (p->s[p->i] == ' ' || p->s[p->i] == '\n' || p->s[p->i] == '\r' || p->s[p->i] == '\t') {
        if (p->s[p->i] == '\n') { p->line++; p->col = 1; }
        else p->col++;
        p->i++;
    }

}

static json_value *json_parse_value(json_parser *p);

static json_value *json_parse_array(json_parser *p) {
    // NOTE: Assume first char is '['
    p->i++; p->col++;
    json_skip_ws(p);
    json_value *arr = json_new(JSON_ARRAY, p->a);
    if (!arr) return NULL;
    if (p->s[p->i] == ']') { p->i++; p->col++; return arr; }
    for (;;) {
        json_value *v = json_parse_value(p);
        if (!v || !json_array_push(arr, v, p->a)) return json_fail(p, "invalid array item");
        json_skip_ws(p);
        if (p->s[p->i] == ',') {
            p->i++; p->col++;
            json_skip_ws(p);
            if (p->s[p->i] == ']') return json_fail(p, "trailing comma not allowed");
            continue;
        }
        if (p->s[p->i] == ']') { p->i++; p->col++; return arr; }
        return json_fail(p, "expected ',' or ']'");
    }
}

static json_value *json_parse_true(json_parser *p) {
    if (strncmp(p->s + p->i, "true", 4) != 0) return json_fail(p, "invalid literal");
    p->i += 4; p->col += 4;
    json_value *v = json_new(JSON_BOOL, p->a);
    if (!v) return json_fail(p, "oom");
    v->as.boolean = 1;
    return v;
}

static json_value *json_parse_false(json_parser *p) {
    if (strncmp(p->s + p->i, "false", 5) != 0) return json_fail(p, "invalid literal");
    p->i += 4; p->col += 4;
    json_value *v = json_new(JSON_BOOL, p->a);
    if (!v) return json_fail(p, "oom");
    v->as.boolean = 0;
    return v;
}

static json_value *json_parse_null(json_parser *p) {
    if (strncmp(p->s + p->i, "null", 4) != 0) return json_fail(p, "invalid literal");
    p->i += 4; p->col += 4;
    return json_new(JSON_NULL, p->a);
}

static json_value *json_parse_number(json_parser *p) {
    /* strict RFC8259 number grammar: -?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)? */
    const char *start = p->s + p->i;
    char *end = NULL;
    if (start[0] == '-') p->i++, p->col++;
    if (p->s[p->i] == '0') { p->i++; p->col++; }
    else if (p->s[p->i] >= '1' && p->s[p->i] <= '9') {
        while (p->s[p->i] >= '0' && p->s[p->i] <= '9') p->i++, p->col++;
    } else return json_fail(p, "invalid number");
    if (p->s[p->i] == '.') {
        p->i++; p->col++;
        if (!(p->s[p->i] >= '0' && p->s[p->i] <= '9')) return json_fail(p, "invalid fracion");
        while (p->s[p->i] >= '0' && p->s[p->i] <= '9') p->i++, p->col++;
    }
    if (p->s[p->i] == 'e' || p->s[p->i] == 'E') {
        p->i++; p->col++;
        if (p->s[p->i] == '+' || p->s[p->i] == '-') p->i++, p->col++;
        if (!(p->s[p->i] >= '0' && p->s[p->i] <= '9')) return json_fail(p, "invalid exponent");
        while (p->s[p->i] >= '0' && p->s[p->i] <= '9') p->i++, p->col++;
    }
    json_value *v = json_new(JSON_NUMBER, p->a);
    if(!v) return json_fail(p, "oom");
    v->as.number = strtod(start, &end);
    if (end != p->s + p->i) { json_free(v, p->a); return json_fail(p, "invalid number"); }
    return v;
}

static json_value *json_parse_value(json_parser *p) {
    json_skip_ws(p);
    switch(p->s[p->i]) {
        case '{': return json_parse_object(p);
        case '[': return json_parse_array(p);
        case '"': return json_parse_string_value(p);
        case 't': return json_parse_true(p);
        case 'f': return json_parse_false(p);
        case 'n': return json_parse_null(p);
        default:  return json_parse_number(p);
    }
}

json_value *json_parse(const char *src, json_allocator *a) {
    json_parser p = { src, 0, 1, 1, a };
    json_value *root = json_parse_value(&p);
    if (!root) return NULL;
    json_skip_ws(&p);
    if (src[p.i] != '\0') { json_free(root, a); return json_fail(&p, "extra characters after JSON value"); }
    return root;
}

void json_free(json_value *v, json_allocator *a) {
    size_t i;
    if (!v) return;
    switch (v->type) {
        case JSON_STRING:
            json_dealloc(a, v->as.string.ptr);
            break;
        case JSON_ARRAY:
            for (i = 0; i < v->as.array.len; i++) json_free(v->as.array.items[i], a);
            json_dealloc(a, v->as.array.items);
            break;
        case JSON_OBJECT:
            for (i = 0; i < v->as.object.len; i++) {
                json_dealloc(a, v->as.object.items[i].key);
                json_free(v->as.object.items[i].val, a);
            }
            json_dealloc(a, v->as.object.items);
            break;
        default: /* null/bool/number */ break;
    }
    json_dealloc(a, v);
}

#endif

/*
   Revision History:

*/

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License

Copyright (c) 2026 Luca Mazza

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)

This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
