#ifndef NIGHTERM_H_
#define NIGHTERM_H_

#include <stdint.h>
#include <stddef.h>

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

struct nighterm_terminal {
    psf2Hdr font_header;
    void *font_data;

    uint8_t cur_x;
    uint8_t cur_y;

#ifdef NIGHTERM_MALLOC_IS_AVAILABLE
    uint32_t *buffer;
#else
    uint32_t buffer[4096*4];
#endif

    char* title;

    uint32_t fg_color;
    uint32_t bg_color;
};

enum nighterm_init_return_codes {
    NIGHTERM_FONT_INVALID = 2,

    NIGHTERM_INVALID_FRAMEBUFFER_ADDRESS = 3,
    NIGHTERM_INVALID_FRAMEBUFFER_SIZE = 4,
    NIGHTERM_INVALID_FRAMEBUFFER_PITCH = 5,
    NIGHTERM_INVALID_FRAMEBUFFER_BPP = 6,

    NIGHTERM_MALLOC_IS_NULL = 7,

    NIGHTERM_SUCCESS = 0
};

struct nighterm_config {
    void *fb_addr;
    uint64_t fb_width;
    uint64_t fb_height;
    uint64_t fb_pitch;
    uint16_t fb_bpp;

    uint32_t terminal_rows;
    uint32_t terminal_cols;

    /* we have only one terminal for now... */
    struct nighterm_terminal terminal[1];
    uint8_t current_terminal;
};

typedef void *(*nighterm_malloc)(size_t);

int nighterm_initialize(void *font, void *framebuffer_addr,
                        uint64_t framebuffer_width, uint64_t framebuffer_height,
                        uint64_t framebuffer_pitch, uint16_t framebuffer_bpp,
                        void *(*custom_malloc)());

void nighterm_write(char ch);
void nighterm_flush(uint8_t r, uint8_t g, uint8_t b);

void nighterm_set_fg_color(uint8_t r, uint8_t g, uint8_t b);
void nighterm_set_bg_color(uint8_t r, uint8_t g, uint8_t b);

void nighterm_set_cursor_position(uint32_t x, uint32_t y);

#endif // NIGHTERM_H_
