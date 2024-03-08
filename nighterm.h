/**
 * @brief This file exports public Nighterm API.
 */

#ifndef NIGHTERM_H_
#define NIGHTERM_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Amount of spaces a tab (or \t) represents.
 */
#define INDENT_AMOUNT 4

#define PSF_MODE 2

#define PSF_MAGIC0 0x72
#define PSF_MAGIC1 0xb5
#define PSF_MAGIC2 0x4a
#define PSF_MAGIC3 0x86

#ifndef NIGHTERM_MAX_TERMINALS
#define NIGHTERM_MAX_TERMINALS 4
#endif

#define NIGHTERM_MAX_RESOLUTION (1920 * 1080)

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
} __attribute__((packed)) psf2Hdr;

/**
 * @brief Nighterm Terminal object.
 */
struct nighterm_terminal
{
  char* name;
  psf2Hdr font_header;
  void* font_data;

  uint8_t cur_x;
  uint8_t cur_y;

  uint32_t rows;
  uint32_t cols;

#ifdef NIGHTERM_MALLOC_IS_AVAILABLE
  uint32_t* backbuffer;
#else
  //uint32_t backbuffer[1920 * (4 * 1080)];
  uint32_t backbuffer; // placeholder
#endif

  uint32_t fg_color;
  uint32_t bg_color;
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

/**
 * @brief Nighterm configuration
 */
struct nighterm_config
{
  void* fb_addr;
  uint64_t fb_width;
  uint64_t fb_height;
  uint64_t fb_pitch;
  uint16_t fb_bpp;

  /* we have only one terminal for now... */
  struct nighterm_terminal terminals[NIGHTERM_MAX_TERMINALS];
  uint8_t terminal_count;
  uint8_t current_terminal;

  /* malloc/free */
  nighterm_malloc malloc;
  nighterm_free free;
};

int
nighterm_initialize(void* font,
                    void* framebuffer_addr,
                    uint64_t framebuffer_width,
                    uint64_t framebuffer_height,
                    uint64_t framebuffer_pitch,
                    uint16_t framebuffer_bpp,
                    nighterm_malloc custom_malloc,
                    nighterm_free custom_free);
void
nighterm_shutdown(void);

int
nighterm_create_terminal(char* name, char* font, uint8_t should_switch);
void
nighterm_switch_terminal(int id);

void
nighterm_write(char c);
void
nighterm_flush(uint8_t r, uint8_t g, uint8_t b);

void
nighterm_set_fg_color(uint8_t r, uint8_t g, uint8_t b);
void
nighterm_set_bg_color(uint8_t r, uint8_t g, uint8_t b);

void
nighterm_move_cursor(int32_t x, int32_t y);
void
nighterm_set_cursor_position(uint32_t x, uint32_t y);
void
nighterm_get_cursor_position(uint32_t* x, uint32_t* y);

int
nighterm_get_current_terminal_id();

#endif // NIGHTERM_H_
