#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "glcd_hal.h"
#include "glcd.h"
#include "uart_streams.h"
#include <assert.h>

/**
 * Converts pixel coordinates to the pixel representation
 * within its block.
 */
#define PIXL(x, y) _BV(y % PX_PER_LINE)

void glcdInit(void) {
    halGlcdInit();
    glcdFillScreen(0x00);
}

/**
 * Swaps the values of x and y.
 */
#define SWAP(x, y) do { uint8_t tmp = x; x = y; y = tmp; } while (false);

/**
 * Bresenham's line algorithm, see
 * http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
void glcdDrawLine(const xy_point p1, const xy_point p2, draw_fn drawPx) {
    uint8_t x0 = p1.x, y0 = p1.y, x1 = p2.x, y1 = p2.y;
    const bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        SWAP(x0, y0);
        SWAP(x1, y1);
    }
    if (x0 > x1) {
        SWAP(x0, x1);
        SWAP(y0, y1);
    }

    const uint8_t deltax = x1 - x0;
    const uint8_t deltay = abs(y1 - y0);
    int16_t error = deltax / 2;
    const int8_t ystep = (y0 < y1 ? 1 : -1);
    int8_t y = y0;

    for (uint8_t x = x0; ; x++) {
        if (steep) {
            drawPx(y, x);
        } else {
            drawPx(x, y);
        }
        error -= deltay;
        if (error < 0) {
            y += ystep;
            error += deltax;
        }

        if (x == x1) {
            break;
        }
    }
}

void glcdFillScreen(const uint8_t pattern) {
    halGlcdSetAddress(0, 0);
    for (int y = 0; y < HEIGHT / PX_PER_LINE; y++) {
        for (int x = 0; x < WIDTH; x++) {
            halGlcdWriteData(pattern);
        }
    }
}

void glcdSetPixel(const uint8_t x, const uint8_t y) {
    if (halGlcdSetAddress(x, y) == 1) {
        return;
    }
    uint8_t px = halGlcdReadData();
    halGlcdSetAddress(x, y);
    halGlcdWriteData(px | PIXL(x, y));
}

void glcdClearPixel(const uint8_t x, const uint8_t y) {
    if (halGlcdSetAddress(x, y) == 1) {
        return;
    }
    halGlcdSetAddress(x, y);
    uint8_t px = halGlcdReadData();
    halGlcdSetAddress(x, y);
    halGlcdWriteData(px & ~PIXL(x, y));
}

void glcdInvertPixel(const uint8_t x, const uint8_t y) {
    if (halGlcdSetAddress(x, y) == 1) {
        return;
    }
    halGlcdSetAddress(x, y);
    uint8_t px = halGlcdReadData();
    halGlcdSetAddress(x, y);
    halGlcdWriteData(px ^ PIXL(x, y));
}

void glcdDrawRect(const xy_point p1, const xy_point p2, draw_fn drawPx) {
    xy_point p1a = { p2.x, p1.y };
    xy_point p2a = { p1.x, p2.y };
    glcdDrawLine(p1, p1a, drawPx);
    glcdDrawLine(p1a, p2, drawPx);
    glcdDrawLine(p2, p2a, drawPx);
    glcdDrawLine(p2a, p1, drawPx);
}

void glcdDrawCircle(const xy_point c, const uint8_t radius, draw_fn drawPx) {
    xy_point p1 = { c.x - radius, c.y - radius };
    xy_point p2 = { c.x + radius, c.y + radius };
    glcdDrawRect(p1, p2, drawPx);

    int16_t f = 1 - radius;
    uint16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    uint8_t x = 0;
    uint8_t y = radius;

    drawPx(c.x, c.y + radius);
    drawPx(c.x, c.y - radius);
    drawPx(c.x + radius, c.y);
    drawPx(c.x - radius, c.y);

    while(x < y)
    {
        if(f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;    
        drawPx(c.x + x, c.y + y);
        drawPx(c.x - x, c.y + y);
        drawPx(c.x + x, c.y - y);
        drawPx(c.x - x, c.y - y);
        drawPx(c.x + y, c.y + x);
        drawPx(c.x - y, c.y + x);
        drawPx(c.x + y, c.y - x);
        drawPx(c.x - y, c.y - x);
    }
}
