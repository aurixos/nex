#ifndef NIGHTERM_H_
#define NIGHTERM_H_

#include "backends/vga.h"
#include "backends/color.h"

#define INDENT_AMOUNT 4

#ifndef PSF2_MODE

#define PSF_MODE 2

#define PSF_MAGIC0 0x72
#define PSF_MAGIC1 0xb5
#define PSF_MAGIC2 0x4a
#define PSF_MAGIC3 0x86

#endif

typedef struct {
    uint8_t magic[4];
    uint32_t version;
    uint32_t headerSize;
    uint32_t flags;
    uint32_t length;
    uint32_t charSize;
    uint32_t height, width;
} __attribute__((packed)) psf2Hdr;

struct Terminal {
    psf2Hdr fonthdr;
    void* fontData;
    int curX;
    int curY;
    int rows;
    int cols;
    int cx;
    int cy;
    ColorCell buffer[4096*4];
    char* title;
};

enum nighterm_init_return_codes {
    NIGHTERM_NO_FONT_SUPPLIED = 1,
    NIGHTERM_FONT_INVALID = 2,
    NIGHTERM_INVALID_FRAMEBUFFER_ADDRESS = 3,
    NIGHTERM_INVALID_FRAMEBUFFER_SIZE = 4,
    NIGHTERM_INVALID_FRAMEBUFFER_PITCH = 5,
    NIGHTERM_INVALID_FRAMEBUFFER_BPP = 6,

    NIGHTERM_SUCCESS = 0
};

struct nighterm_fbinfo {
    void *addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
};

extern struct Terminal term;

extern struct nighterm_fbinfo fbinfo;

int init_nighterm(void *font,
                void *framebuffer_addr,
                uint64_t framebuffer_width,
                uint64_t framebuffer_height,
                uint64_t framebuffer_pitch,
                uint16_t framebuffer_bpp);
void nighterm_refresh();
void nighterm_clear();
void nighterm_set_char_fg(uint8_t r, uint8_t b, uint8_t g);
void nighterm_set_char_bg(uint8_t r, uint8_t b, uint8_t g);
void nighterm_write(char ch);
void nighterm_move_cursor(int row, int col);

#endif // NIGHTERM_H_