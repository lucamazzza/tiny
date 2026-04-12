<p align='center'>
  
<img src="https://capsule-render.vercel.app/api?type=venom&color=323b9d&height=200&section=header&text=tiny&fontSize=90&fontColor=ffffff&animation=fadeIn&fontAlignY=35&desc=A%20collection%20of%20single-file%20libraries&descAlignY=61&descAlign=50"/>

</p>

Single-file public domain libraries for C/C++

## FAQ

### How are tiny libraries licensed?

These libraries are in the public domain. You can do anything you want with them. You have no legal obligation
to do anything else, although I appreciate attribution.

They are also licensed under the MIT open source license, if you have lawyers who are not happy with the public
domain. Every source file includes explicitly dual licenses for you to choose from.

> [!IMPORTANT]
> Because the libraries come with no obligation on your behalf, they also provide no warranty.

### If I wrap a tiny library in a new library, does the new library have to be public domain?

No.

### Do I have to include the entire repo when I include a tiny library?

You don't have to include the `README` or the license in your binary distribution, but you need to keep the
header file, which includes the license statement.

### How do I use these libraries?

For each library, yout do this in exactly one C or C++ file:

```c
#define [LIBNAME]_IMPLEMENTATION
#include "library_name.h"
```

if you whish to use the implementation provided, and not reimplement it.

For example:

```c
#define GL_IMPLEMENTATION
#include "tiny_gl.h"
```

You can then include the header normally in other files.

### Why single-file headers?

1. Easy to integrate: just drop it in your project
2. No build configuration needed
3. No dependency hassles
4. Easy to vendor and version control
5. Follows the STB philosophy

### Where is the documentation?

The documentation is at the top of each header file. Simply open the `.h` file in your code editor and read
the comments.

### I found a bug / I have a feature request

Open an issue on the Github repository.

## Libraries

| Name                       | Description                                     | Use case                              |
|----------------------------|-------------------------------------------------|---------------------------------------|
| [tiny_gl.h](tiny_gl.h)     | Software 3D Rasterizer, OpenGL-Style, No GPU    | Embedded Graphics, Rendering Fallback |
| [tiny_hook.h](tiny_hook.h) | Callback/Event System, Lightweight, Macro-Based | Embedded Events, Simple Callbacks     |

## Philosophy

These libraries follow a strict philosophy:

1. Easy to integrate: single file, drop it in your project
2. Easy to use: simple APIs that do one thing well
3. Easy to mantain: minimal dependencies, readable code
4. Easy to debug: full source code right there
5. Liberal licensing: public domain, use anywhere

The libraries prioritize simplicity and portability over performance. They are meant to be useful
for small projects, embedded systems, learning and situations where you want something that just
works without complex build systems or dependencies.

## Credits

Written by Luca Mazza - Inspired by Sean Barret's STB ([https://github.com/nothings/stb](https://github.com/nothings/stb))
