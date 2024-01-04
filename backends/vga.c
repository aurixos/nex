#include "../nighterm.h"

void nighterm_putpixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b)
{
    *(uint32_t*)(fbinfo.addr + x * (fbinfo.bpp >> 3) + y * fbinfo.pitch) = (0xFF << 24) | (r << 16) | (g << 8) | b;
}

void nighterm_flush(uint16_t red, uint16_t green, uint16_t blue)
{
    for (uint64_t y = 0; y < fbinfo.height; y++) {
        for (uint64_t x = 0; x < fbinfo.width; x++) {
            nighterm_putpixel(x,y, red, green, blue);
        }
    }
}
