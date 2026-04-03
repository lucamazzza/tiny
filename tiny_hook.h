/* tiny_hook - v1.00 - public domain graphics library - https://mazluc.ch/tiny
                                   no warranty implied; use at your own risk

   Do this:
      #define HOOK_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the
   implementation

   // It should look like this:
   #include ...
   #define HOOK_IMPLEMENTATION
   #include "tiny_hook.h"

   QUICK NOTES:
      - Header-only, macro-based hook/callback system
      - Fixed array, no malloc, no threading
      - Define hooks with hook_def(name, return_type, arg_types...)
      - Max handlers per hook: HOOK_MAX (default 16)
      - Assert on overflow (disable with HOOK_NO_ASSERT)

   EXAMPLE:
      hook_def(on_click, void, int, int)

      typedef struct {
          hook_t(on_click) clicked;
      } Button;

      void my_handler(int x, int y) { printf("%d %d\n", x, y); }

      Button btn;
      hook_init(&btn.clicked);
      hook_add(&btn.clicked, my_handler);
      hook_call(&btn.clicked, 10, 20);
      hook_clear(&btn.clicked);

      Full documentation under DOCUMENTATION below.

   LICENSE:

      See end of file for license information.

   RECENT REVISION HISTORY:

      1.00  (2026-04-03) first release

      See end of file for full revision history.

   ============================    Contributors    =========================

   State machine & Structure:
      Luca Mazza
*/

#ifndef HOOK_DEFINITIONS
#define HOOK_DEFINITIONS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HOOK_NO_STRING
#include <string.h>
#endif

#ifndef HOOK_NO_ASSERT
#include <assert.h>
#endif

#ifndef HOOK_MAX
#define HOOK_MAX 16
#endif

#define hook_def(name, ret, ...)             \
    typedef ret (*name##_fn_t)(__VA_ARGS__); \
    typedef struct {                         \
        name##_fn_t handlers[HOOK_MAX];      \
        int         cnt;                     \
    } name##_hook_t

#define hook_t(name) name##_hook_t

#define hook_init(h)                                     \
    do {                                                 \
        (h)->cnt = 0;                                    \
        memset((h)->handlers, 0, sizeof((h)->handlers)); \
    } while (0)

#define hook_add(h, fn)                                 \
    do {                                                \
        assert((h)->cnt < HOOK_MAX && "hook overflow"); \
        (h)->handlers[(h)->cnt++] = (fn);               \
    } while (0)

#define hook_call(h, ...)                                          \
    do {                                                           \
        for (int _i = 0; _i < (h)->cnt; _i++)                      \
            if ((h)->handlers[_i]) (h)->handlers[_i](__VA_ARGS__); \
    } while (0)

#define hook_del(h, fn)                                                                               \
    do {                                                                                              \
        for (int _i = 0; _i < (h)->cnt; _i++) {                                                       \
            if ((h)->handlers[_i] == (fn)) {                                                          \
                for (int _j = _i; _j < (h)->cnt - 1; _j++) (h)->handlers[_j] = (h)->handlers[_j + 1]; \
                (h)->cnt--;                                                                           \
                break;                                                                                \
            }                                                                                         \
        }                                                                                             \
    } while (0)

#define hook_clear(h) hook_init(h)

#define hook_has(h) ((h)->cnt > 0)

#define hook_cnt(h) ((h)->cnt)

#define hook_full(h) ((h)->cnt >= HOOK_MAX)

#define hook_foreach(h, idx)                 \
    for (int idx = 0; idx < (h)->cnt; idx++) \
        if ((h)->handlers[idx])

#ifdef __cplusplus
}
#endif

#endif

#ifdef HOOK_IMPLEMENTATION

// No implementation! The library is Macro-Based

#endif

/*
   Revision History:

      1.00  (2026-04-03) first release
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
