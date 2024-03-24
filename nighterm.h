/**
 * @brief This file exports public Nighterm API.
 */

#ifndef NIGHTERM_H
#define NIGHTERM_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Amount of spaces a tab (or \t) represents.
 */
#ifndef NIGHTERM_INDENT_WIDTH
#define NIGHTERM_INDENT_WIDTH 4
#endif

#define PSF_MODE 2

#define PSF_MAGIC0 0x72
#define PSF_MAGIC1 0xb5
#define PSF_MAGIC2 0x4a
#define PSF_MAGIC3 0x86

/**
 * @brief Memory allocator function pointer.
 */
typedef void* (*nighterm_malloc)(size_t);

/**
 * @brief Memory allocator function pointer.
 */
typedef void (*nighterm_free)(void*);

/**
 * @brief PSF2 file header.
 */
typedef struct
{
  uint8_t magic[4];
  uint32_t version;
  uint32_t headerSize;
  uint32_t flags;
  uint32_t length;
  uint32_t charSize;
  uint32_t height, width;
} __attribute__((packed)) psf2_header;

/**
 * @brief Nighterm Terminal object.
 */
struct nighterm_ctx
{
  void *fb_addr;
  uint64_t fb_width;
  uint64_t fb_height;
  uint64_t fb_bpp;
  uint64_t fb_pitch;

  psf2_header font_header;
  void* font_data;

  uint8_t cur_x;
  uint8_t cur_y;

  uint32_t rows;
  uint32_t cols;

  uint32_t fg_color;
  uint32_t bg_color;

  nighterm_malloc malloc;
  nighterm_free free;
};

/**
 * @brief Return codes
 */
enum nighterm_status
{
  NIGHTERM_MALLOC_IS_NULL = 1,

  NIGHTERM_FONT_INVALID = -2,

  NIGHTERM_INVALID_FRAMEBUFFER_ADDRESS = -3,
  NIGHTERM_INVALID_FRAMEBUFFER_SIZE = -4,
  NIGHTERM_INVALID_FRAMEBUFFER_PITCH = -5,
  NIGHTERM_INVALID_FRAMEBUFFER_BPP = -6,

  NIGHTERM_NO_MORE_MEMORY = -7,

  NIGHTERM_SUCCESS = 0
};

int
nighterm_initialize(struct nighterm_ctx *new,
                    void* font,
                    void* framebuffer_addr,
                    uint64_t framebuffer_width,
                    uint64_t framebuffer_height,
                    uint64_t framebuffer_pitch,
                    uint16_t framebuffer_bpp,
                    nighterm_malloc custom_malloc,
                    nighterm_free custom_free);
void
nighterm_shutdown(struct nighterm_ctx *context);

int
nighterm_create_terminal(char* name, char* font, uint8_t should_switch);
void
nighterm_switch_terminal(int id);

int
nighterm_set_font(struct nighterm_ctx *context, void *font);

void
nighterm_write(struct nighterm_ctx *context, char c);
void
nighterm_flush(struct nighterm_ctx *context, uint8_t r, uint8_t g, uint8_t b);

void
nighterm_set_fg_color(struct nighterm_ctx *context, uint8_t r, uint8_t g, uint8_t b);
void
nighterm_set_bg_color(struct nighterm_ctx *context, uint8_t r, uint8_t g, uint8_t b);

void
nighterm_move_cursor(struct nighterm_ctx *context, int32_t x, int32_t y);
void
nighterm_set_cursor_position(struct nighterm_ctx *context, uint32_t x, uint32_t y);
void
nighterm_get_cursor_position(struct nighterm_ctx *context, uint32_t* x, uint32_t* y);

#endif // NIGHTERM_H
