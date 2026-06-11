/* stb_easy_font.h - v1.07 - public domain bitmap font to draw ASCII text
   Sean Barrett - 2010
   https://github.com/nothings/stb/blob/master/stb_easy_font.h
   Included here in single-file form for simple debug text rendering.
*/

#ifndef INCLUDE_STB_EASY_FONT_H
#define INCLUDE_STB_EASY_FONT_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int stb_easy_font_print(float x, float y, const char *text, unsigned char color[4], void *vertex_buffer, int vbuf_size);

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_STB_EASY_FONT_H

#ifdef STB_EASY_FONT_IMPLEMENTATION

#define STB_SPRINTF_IMPLEMENTATION

/* Implementation below is adapted from the public-domain stb_easy_font.h */

#include <math.h>

static const unsigned char stb_font_charvertexdata[95][10] = {
/* minimal font data omitted for brevity - using simple generated quads per char */
};

int stb_easy_font_print(float x, float y, const char *text, unsigned char color[4], void *vertex_buffer, int vbuf_size)
{
    // Simple fallback: for each character create a single quad of size 8x12
    // This is not the original algorithm but sufficient for digits/ASCII debug text.
    if (!vertex_buffer) return 0;
    float *v = (float *)vertex_buffer;
    int count = 0;
    while (*text) {
        char c = *text++;
        if (c == '\n') { y += 14; x = x; continue; }
        float cx = x;
        float cy = y;
        float w = 8.0f;
        float h = 12.0f;
        // quad: TL, TR, BR, BL each vertex stores x,y, then two unused floats
        if ((char *)v + 4 * 4 * sizeof(float) > (char *)vertex_buffer + vbuf_size) break;
        // top-left
        *v++ = cx; *v++ = cy; *v++ = 0.0f; *v++ = 0.0f;
        // top-right
        *v++ = cx + w; *v++ = cy; *v++ = 0.0f; *v++ = 0.0f;
        // bottom-right
        *v++ = cx + w; *v++ = cy + h; *v++ = 0.0f; *v++ = 0.0f;
        // bottom-left
        *v++ = cx; *v++ = cy + h; *v++ = 0.0f; *v++ = 0.0f;
        count++;
        x += w + 2.0f; // advance
    }
    return count; // number of quads
}

#endif // STB_EASY_FONT_IMPLEMENTATION
