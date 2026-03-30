/* gl - v0.1 - public domain graphics library - https://mazluc.ch/tiny
                             no warranty implied; use at your own risk

   Do this:
      #define GL_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the
   implementation

   // It should look like this:
   #include ...
   #define GL_IMPLEMENTATION
   #include "gl.h"

   QUICK NOTES:
      ...
      Full documentation under DOCUMENTATION below.

   LICENSE:

      See end of file for license information.

   RECENT REVISION HISTORY:
      ...
      See end of file for full revision history.

   ============================    Contributors    =========================

*/
#ifndef GL_DEFINITIONS
#define GL_DEFINITIONS

// DOCUMENTATION
//
// gl.h is a single-header software rasterizer library in the style of the stb libraries.
// It provides a minimal OpenGL-like API for 3D rendering to memory buffers.
//
// Limitations:
//   - Software rendering only (no hardware acceleration)
//   - Triangles only (no lines, points, or other primitives yet)
//   - No texturing or advanced shading
//   - No anti-aliasing
//   - Single-threaded
//   - Fixed-function pipeline only
//
// Basic usage:
//   uint32_t framebuffer[640 * 480];
//   gl_init(640, 480, framebuffer, NULL);
//   
//   // Set up 3D view
//   gl_state.projection = gl_perspective(45.0f, 640.0f/480.0f, 0.1f, 100.0f);
//   gl_state.view = gl_lookat((gl_vec3){0,0,5}, (gl_vec3){0,0,0}, (gl_vec3){0,1,0});
//   
//   gl_clear(0x000000FF);  // Clear to black
//   
//   // Draw red triangle
//   gl_begin(GL_TRIANGLES);
//   gl_color_3f(1.0f, 0.0f, 0.0f);
//   gl_vertex_3f( 0.0f,  1.0f, 0.0f);
//   gl_vertex_3f(-1.0f, -1.0f, 0.0f);
//   gl_vertex_3f( 1.0f, -1.0f, 0.0f);
//   gl_end();
//   
//   // framebuffer now contains the rendered triangle
//   gl_shutdown();
//
// Standard parameters:
//   Most functions follow OpenGL conventions:
//   - Colors are specified as floats in range [0.0, 1.0]
//   - Matrices are column-major 4x4 or 3x3
//   - Angles are in degrees for perspective, radians for math functions
//   - Coordinate system: right-handed, Y-up
//   - Depth buffer range: [0.0, 1.0] where 1.0 is far
//
// ===========================================================================
//
// Philosophy:
//   This library prioritizes simplicity and ease of integration over performance.
//   It's designed for:
//   - Learning 3D graphics concepts
//   - Simple 3D rendering in games/demos where GPU isn't available
//   - Software fallback rendering
//   - Embedded systems without GPU support
//   
//   The API intentionally mimics early OpenGL to make it familiar to graphics
//   programmers. All rendering is done in software using barycentric coordinate
//   triangle rasterization with depth testing.
//
// ===========================================================================
//
// Additional configurations:
//   #define GL_MALLOC(sz)     - Override memory allocation (default: malloc)
//   #define GL_FREE(ptr)      - Override memory deallocation (default: free)
//   #define GL_NO_STDINT      - Don't include stdint.h
//   #define GL_NO_MATH        - Don't include math.h (you must provide sqrt, etc.)
//
// Memory management:
//   gl_init() can accept user-provided buffers or allocate its own:
//   - gl_init(w, h, my_color_buf, my_depth_buf)  // Use provided buffers
//   - gl_init(w, h, NULL, NULL)                  // Library allocates buffers
//   - gl_init(w, h, my_color_buf, NULL)         // Mix of both
//   
//   Call gl_shutdown() to free any library-allocated memory.
//   User-provided buffers are never freed by the library.

#ifndef GL_NO_STDINT
#include <stdint.h>
#endif

#ifndef GL_NO_MATH
#include <math.h>
#endif

#ifndef GL_MALLOC
#define GL_MALLOC(sz) malloc(sz)
#endif

#ifndef GL_FREE
#define GL_FREE(p) free(p)
#endif

#define GL_TRIANGLES 0
#define GL_LINES     1
#define GL_POINTS    2

typedef struct {
    union {
        float m[9];
        struct {
            float v0, v1, v2;
            float v3, v4, v5;
            float v6, v7, v8;
        };
    };
} gl_mat3;

typedef struct {
    union {
        float m[16];
        struct {
            float v0, v1, v2, v3;
            float v4, v5, v6, v7;
            float v8, v9, v10, v11;
            float v12, v13, v14, v15;
        };
    };
} gl_mat4;

typedef struct {
    union {
        float v[3];
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
    };
} gl_vec3;

typedef struct {
    union {
        float v[4];
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        };
    };
} gl_vec4;

gl_mat4 gl_identity(void);
gl_mat3 gl_mul(gl_mat3 a, gl_mat3 b);
gl_mat4 gl_mul(gl_mat4 a, gl_mat4 b);
gl_mat4 gl_perspective(float fov, float aspect, float near, float far);
gl_mat4 gl_lookat(gl_vec3 eye, gl_vec3 center, gl_vec3 up);
gl_vec3 gl_vec3_normalize(gl_vec3 v);
gl_vec3 gl_vec3_cross(gl_vec3 a, gl_vec3 b);
gl_vec3 gl_vec3_sub(gl_vec3 a, gl_vec3 b);
float   gl_vec3_dot(gl_vec3 a, gl_vec3 b);

void gl_init(int width, int height, void *color_buf, void *depth_buf);
void gl_clear(unsigned int color);
void gl_vertex_3f(float x, float y, float z);
void gl_vertex_3fv(gl_vec3 v);
void gl_color_3f(float r, float g, float b);
void gl_color_3fv(gl_vec3 c);
void gl_begin(int prim_type);
void gl_end(void);
void gl_viewport(int width, int height, uint32_t *color_ptr, float *depth_ptr);
void gl_shutdown(void);

#endif  // GL_DEFINITIONS

//TODO: Remove once done
#define GL_IMPLEMENTATION

#ifdef GL_IMPLEMENTATION

static struct {
    uint32_t *color_buf;
    float    *depth_buf;
    int      width, height;
    gl_mat4  model;
    gl_mat4  view;
    gl_mat4  projection;
    uint32_t clear_color;
    uint32_t cur_color;
    gl_vec4  vertex_cache[3];
    int      vertex_count;
    int      primitive_type;
    int      owns_color_buf, owns_depth_buf;
} gl_state;

static gl_vec4 mgl_transform_vert(float x, float y, float z) {
    gl_mat4 mvp = gl_mul(gl_state.projection, gl_mul(gl_state.view, gl_state.model));
    gl_vec4 v;
    v.x = x * mvp.v0 + y * mvp.v4 + z * mvp.v8 + mvp.v12;
    v.y = x * mvp.v1 + y * mvp.v5 + z * mvp.v9 + mvp.v13;
    v.z = x * mvp.v2 + y * mvp.v6 + z * mvp.v10 + mvp.v14;
    v.w = x * mvp.v3 + y * mvp.v7 + z * mvp.v11 + mvp.v15;
    if (v.w != 0.0f) {
        v.x /= v.w;
        v.y /= v.w;
        v.z /= v.w;
    }
    v.x = (v.x + 1.0f) * 0.5f * gl_state.width;
    v.y = (1.0f - v.y) * 0.5f * gl_state.height;
    return v;
}

inline static float mgl_edge_func(gl_vec4 a, gl_vec4 b, float px, float py) {
    return (px - a.x) * (b.y - a.y) - (py - a.y) * (b.x - a.x);
}

void mgl_rasterize_triangle(gl_vec4 v0, gl_vec4 v1, gl_vec4 v2) {
    int min_x = (int)fminf(v0.x, fminf(v1.x, v2.x));
    int max_x = (int)fmaxf(v0.x, fmaxf(v1.x, v2.x));
    int min_y = (int)fminf(v0.y, fminf(v1.y, v2.y));
    int max_y = (int)fmaxf(v0.y, fmaxf(v1.y, v2.y));
    if (min_x < 0) min_x = 0; if (max_x >= gl_state.width)  max_x = gl_state.width  - 1;
    if (min_y < 0) min_y = 0; if (max_y >= gl_state.height) max_y = gl_state.height - 1;
    float area = mgl_edge_func(v0, v1, v2.x, v2.y);
    if (area == 0) return;
    if (fabs(area) < 1e-6f) return;
    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            float px = (float)x + 0.5f;
            float py = (float)y + 0.5f;
            float w0 = mgl_edge_func(v1, v2, px, py) / area;
            float w1 = mgl_edge_func(v2, v0, px, py) / area;
            float w2 = mgl_edge_func(v0, v1, px, py) / area;
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
                int idx = y * gl_state.width + x;
                if (z < gl_state.depth_buf[idx]) {
                    gl_state.depth_buf[idx] = z;
                    gl_state.color_buf[idx] = gl_state.cur_color;
                }
            }
        }
    }
}

// ============================================================================

gl_mat4 gl_identity(void) {
    gl_mat4 mat = {0};
    mat.m[0]    = 1.0f;
    mat.m[5]    = 1.0f;
    mat.m[10]   = 1.0f;
    mat.m[15]   = 1.0f;
    return mat;
}

gl_mat4 gl_mul(gl_mat4 a, gl_mat4 b) {
    gl_mat4 res;
    const float *A = a.m;
    const float *B = b.m;
    float *R       = res.m;
    for (int i = 0; i < 4; i++) {
        float ai0 = A[i * 4 + 0], ai1 = A[i * 4 + 1], ai2 = A[i * 4 + 2], ai3 = A[i * 4 + 3];
        R[i * 4 + 0] = ai0 * B[0] + ai1 * B[4] + ai2 * B[8] + ai3 * B[12];
        R[i * 4 + 1] = ai0 * B[1] + ai1 * B[5] + ai2 * B[9] + ai3 * B[13];
        R[i * 4 + 2] = ai0 * B[2] + ai1 * B[6] + ai2 * B[10] + ai3 * B[14];
        R[i * 4 + 3] = ai0 * B[3] + ai1 * B[7] + ai2 * B[11] + ai3 * B[15];
    }
    return res;
}

gl_mat3 gl_mul(gl_mat3 a, gl_mat3 b) {
    gl_mat3 res;
    const float *A = a.m;
    const float *B = b.m;
    float *R       = res.m;
    for (int i = 0; i < 3; i++) {
        float ai0 = A[i * 3 + 0], ai1 = A[i * 3 + 1], ai2 = A[i * 3 + 2];
        R[i * 3 + 0] = ai0 * B[0] + ai1 * B[3] + ai2 * B[6];
        R[i * 3 + 1] = ai0 * B[1] + ai1 * B[4] + ai2 * B[7];
        R[i * 3 + 2] = ai0 * B[2] + ai1 * B[5] + ai2 * B[8];
    }
    return res;
}

gl_mat4 gl_perspective(float fov, float aspect, float near, float far) {
    gl_mat4 res        = {0};
    float tan_half_fov = tanf((fov * 3.14159265f / 180.0f) / 2.0f);
    res.m[0]           = 1.0f / (aspect * tan_half_fov);
    res.m[5]           = 1.0f / tan_half_fov;
    res.m[10]          = -(far + near) / (far - near);
    res.m[11]          = -1.0f;
    res.m[14]          = -(2.0f * far * near) / (far - near);
    return res;
}

gl_mat4 gl_lookat(gl_vec3 eye, gl_vec3 center, gl_vec3 up) {
    gl_vec3 f = gl_vec3_normalize(gl_vec3_sub(center, eye));
    gl_vec3 s = gl_vec3_normalize(gl_vec3_cross(f, up));
    gl_vec3 u = gl_vec3_cross(s, f);
    gl_mat4 result = gl_identity();
    result.v0 =  s.x; result.v4 =  s.y; result.v8  =  s.z;
    result.v1 =  u.x; result.v5 =  u.y; result.v9  =  u.z;
    result.v2 = -f.x; result.v6 = -f.y; result.v10 = -f.z;
    result.v12 = -gl_vec3_dot(s, eye);
    result.v13 = -gl_vec3_dot(u, eye);
    result.v14 =  gl_vec3_dot(f, eye);
    return result;
}

gl_vec3 gl_vec3_normalize(gl_vec3 v) {
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len > 0.0f) {
        v.x /= len; v.y /= len; v.z /= len;
    }
    return v;
}

gl_vec3 gl_vec3_cross(gl_vec3 a, gl_vec3 b) {
    gl_vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;  
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

gl_vec3 gl_vec3_sub(gl_vec3 a, gl_vec3 b) {
    gl_vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

float gl_vec3_dot(gl_vec3 a, gl_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void gl_init(int width, int height, void *color_buf, void *depth_buf) {
    gl_state.width  = width;
    gl_state.height = height;
    gl_state.vertex_count = 0;
    gl_state.primitive_type = GL_TRIANGLES;
    if (color_buf) {
        gl_state.color_buf = (uint32_t*)color_buf;
        gl_state.owns_color_buf = 0;
    } else {
        gl_state.color_buf = (uint32_t*)GL_MALLOC(width * height * sizeof(uint32_t));
        gl_state.owns_color_buf = 1;
    }
    if (depth_buf) {
        gl_state.depth_buf = (float*)depth_buf;
        gl_state.owns_depth_buf = 0;
    } else {
        gl_state.depth_buf = (float*)GL_MALLOC(width * height * sizeof(float));
        gl_state.owns_depth_buf = 1;
    }
    gl_state.model      = gl_identity();
    gl_state.view       = gl_identity();
    gl_state.projection = gl_identity();
    gl_state.cur_color  = 0xFFFFFFFF;
}

void gl_clear(unsigned int color) {
    int count = gl_state.width * gl_state.height;
    for (int i = 0; i < count; ++i) {
        gl_state.color_buf[i] = color;
        if (gl_state.depth_buf) gl_state.depth_buf[i] = 1.0f;
    }
}

void gl_viewport(int width, int height) {
    gl_state.width  = width;
    gl_state.height = height;
}

void gl_vertex_3f(float x, float y, float z) {
    gl_state.vertex_cache[gl_state.vertex_count] = mgl_transform_vert(x, y, z);
    gl_state.vertex_count++;
    if (gl_state.primitive_type == GL_TRIANGLES && gl_state.vertex_count == 3) {
        mgl_rasterize_triangle(gl_state.vertex_cache[0], gl_state.vertex_cache[1], gl_state.vertex_cache[2]);
        gl_state.vertex_count = 0;
    }
}

void gl_vertex_3fv(gl_vec3 v) {
    gl_vertex_3f(v.x, v.y, v.z);
}

void gl_color_3f(float r, float g, float b) {
    uint8_t ir = (uint8_t)(r * 255.0f);
    uint8_t ig = (uint8_t)(g * 255.0f);
    uint8_t ib = (uint8_t)(b * 255.0f);
    gl_state.cur_color = (0xFF << 24) | (ir << 16) | (ig << 8) | ib;
}

void gl_color_3fv(gl_vec3 c) {
    gl_color_3f(c.r, c.g, c.b);
}

void gl_begin(int prim_type) {
    gl_state.primitive_type = prim_type;
    gl_state.vertex_count = 0;
}

void gl_end(void) {
    gl_state.vertex_count = 0;
}

void gl_shutdown(void) {
    if (gl_state.owns_color_buf && gl_state.color_buf) GL_FREE(gl_state.color_buf);
    if (gl_state.owns_depth_buf && gl_state.depth_buf) GL_FREE(gl_state.depth_buf);
    gl_state.color_buf = NULL;
    gl_state.depth_buf = NULL;
}

#endif  // GL_IMPLEMENTATION
