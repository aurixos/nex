#include "vga.h"
#include "../nighterm.h"

uint16_t display_red = 0;
uint16_t display_green = 0;
uint16_t display_blue = 0;

int getScreenHeight()
{
    return fbinfo.height;
}

int getScreenWidth()
{
    return fbinfo.width;
}

void display_write_data(uint32_t address, uint8_t red, uint8_t green, uint8_t blue)
{
    (uint64_t)(fbinfo.addr)[address] = rgb(red, green, blue);
}

unsigned int rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    return (0xFF << 24) | (red << 16) | (green << 8) | blue;
}

void draw_pixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t* fb_ptr = (uint32_t*)((uintptr_t)fbinfo.addr + x * 4 + y * fbinfo.pitch);
    *fb_ptr = rgb(r, g, b);
}

void set_background_color(uint16_t red, uint16_t green, uint16_t blue)
{
    for (int y = 0; y < getScreenHeight(); y++) {
        for (int x = 0; x < getScreenWidth(); x++) {
            draw_pixel(x,y, red, green, blue);
        }
    }
}
