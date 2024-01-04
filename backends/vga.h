#ifndef VGA_H_
#define VGA_H_

#include <stdint.h>
#include <stddef.h>

void nighterm_putpixel(uint64_t x, uint64_t y, uint8_t r, uint8_t b, uint8_t g);
void nighterm_flush(uint16_t red, uint16_t green, uint16_t blue);

uint32_t decode_color(uint32_t color);
uint32_t decode_fg_color(uint32_t fg_color);
uint32_t decode_bg_color(uint32_t bg_color);

#endif // VGA_H_