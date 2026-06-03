/* tiny_appwrite - v1.00 - public domain C/C++ Appwrite SDK - https://mazluc.ch/tiny
                                           no warranty implied; use at your own risk

   Do this:
      #define APPWRITE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the
   implementation

   // It should look like this:
   #include ...
   #define APPWRITE_IMPLEMENTATION
   #include "tiny_appwrite.h"

   QUICK NOTES:

      Primarly of interest to 

      Full documentation under DOCUMENTATION below.

   LICENSE:

      See end of file for license information.

   RECENT REVISION HISTORY:


      See end of file for full revision history.

   ============================    Contributors    =========================

*/

#ifndef APPWRITE_DEFINITIONS
#define APPWRITE_DEFINITIONS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef APPWRITE_NO_STDINT
#include <string.h>
#endif

#ifndef APPWRITE_NO_STDDEF
#include <stddef.h>
#endif

typedef struct ta_client ta_client_t;

struct ta_connection_config {
    const char *endpoint;
    const char *project_id;
    const char *db_id;
    const char *collection_id;
};

typedef void (*ta_response_cb)(int status_code, const char *response_body, void *user_data);

ta_client_t *ta_client_init(void);
void ta_client_free(ta_client_t *client);

void ta_client_create_document(
    ta_client_t *client,
    const struct ta_connection_config *config,
    const char *json_data,
    ta_response_cb callback,
    void *user_data
);

// TODO: add all features (server and client)

#ifdef __cplusplus
}
#endif

#endif // APPWRITE_DEFINITIONS

#ifdef APPWRITE_IMPLEMENTATION

// TODO: it'

#endif // APPWRITE_IMPLEMENTATION

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
