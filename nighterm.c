/**
 * @brief This file implements public Nighterm API.
 */

#include "nighterm.h"
#include "nighterm_font.h"

/**
 * @brief Draws a pixel to backbuffer of the current terminal.
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          x
 *                 X position
 *
 * @param          y
 *                 Y position
 *
 * @param          r
 *                 Red color value (0-255)
 *
 * @param          g
 *                 Green color value (0-255)
 *
 * @param          b
 *                 Blue color value (0-255)
 */
static void
nighterm_priv___putpixel(struct nighterm_ctx *context,
                         uint64_t x,
                         uint64_t y,
                         uint8_t r,
                         uint8_t g,
                         uint8_t b)
{
  context->backbuffer[x * (context->fb_bpp >> 3) + y * context->fb_pitch] = (0xFF << 24) | (r << 16) | (g << 8) | b;
  // *(uint32_t *)(context->fb_addr + x * (context->fb_bpp >> 3) +
  //              y * context->fb_pitch) = (0xFF << 24) | (r << 16) | (g << 8) | b;
}

/**
 * @brief Parses a PSF2 font.
 *
 * @param          font
 *                 PSF2 font buffer
 *
 * @param ptr      header
 *                 Pointer to a PSF2 header structure which will be written to.
 *
 * @param ptr      data
 *                 Pointer to a a memory where font data will be written to.
 */
static int
nighterm_priv___parse_psf2_font(char* font, psf2_header* header, void** data)
{
  char* psf2buf = font;
  *header = *(psf2_header *)font;
  psf2buf += header->headerSize;
  *data = psf2buf;

  if (PSF_MODE == 2 || PSF_MODE == 1
        ? (header->magic[0] != PSF_MAGIC0 || header->magic[1] != PSF_MAGIC1 ||
           header->magic[2] != PSF_MAGIC2 || header->magic[3] != PSF_MAGIC3)
        : 0) {
    return NIGHTERM_FONT_INVALID;
  }

  return NIGHTERM_SUCCESS;
}

/**
 * @brief Initializes Nighterm terminals and configuration.
 *
 * @param          config
 *                 Pointer to a new context structure.
 *
 * @param optional font
 *                 Pointer to a PSF2 font buffer. If NULL, default font is used.
 *
 * @param          framebuffer_addr
 *                 Framebuffer address
 *
 * @param          framebuffer_width
 *                 Framebuffer width in pixels
 *
 * @param          framebuffer_height
 *                 Framebuffer height in pixelss
 *
 * @param          framebuffer_pitch
 *                 Framebuffer Pitch
 *
 * @param          framebuffer_bpp
 *                 Framebuffer's Bits per Pixel
 *
 * @param optional custom_malloc
 *                 Pointer to a malloc() function
 *                 provided by the host OS
 *
 * @return         NIGHTERM_SUCCESS if the initialization was successful;
 *                 error code otherwise. All error codes are defined in
 * nighterm.h.
 */
int
nighterm_initialize(struct nighterm_ctx *config,
                    void *font,
                    void *framebuffer_addr,
                    uint64_t framebuffer_width,
                    uint64_t framebuffer_height,
                    uint64_t framebuffer_pitch,
                    uint16_t framebuffer_bpp,
                    nighterm_malloc custom_malloc,
                    nighterm_free custom_free)
{
  if (font == NULL) {
    /* No font supplied. */
    font = &nighterm_default_font;
  }

  if (framebuffer_addr == NULL) {
    /* No framebuffer specified, or invalid address. */
    return NIGHTERM_INVALID_FRAMEBUFFER_ADDRESS;
  }

  if (framebuffer_width < 1 || framebuffer_height < 1) {
    /* Invalid framebuffer dimensions. */
    return NIGHTERM_INVALID_FRAMEBUFFER_SIZE;
  }

  if (framebuffer_pitch < 1) {
    /* Invalid framebuffer pitch. */
    return NIGHTERM_INVALID_FRAMEBUFFER_PITCH;
  }

  if (framebuffer_bpp < 1) {
    /* Invalid framebuffer bpp. */
    return NIGHTERM_INVALID_FRAMEBUFFER_BPP;
  }

  config->fb_addr = framebuffer_addr;
  config->fb_width = framebuffer_width;
  config->fb_height = framebuffer_height;
  config->fb_pitch = framebuffer_pitch;
  config->fb_bpp = framebuffer_bpp;

#ifdef NIGHTERM_MALLOC_IS_AVAILABLE
  if (custom_malloc == NULL || custom_free == NULL) {
    return NIGHTERM_INVALID_PARAMETERS;
  }
  config->malloc = custom_malloc;
  config->free = custom_free;
#else
  (void)custom_malloc;
  (void)custom_free;
  config->malloc = NULL;
  config->free = NULL;
#endif


#ifdef NIGHTERM_MALLOC_IS_AVAILABLE
  // TODO: Allocate memory for backbuffer
#endif

  int status =
    nighterm_priv___parse_psf2_font(font,
                                    &config->font_header,
                                    &config->font_data);
  if (status != NIGHTERM_SUCCESS) {
#ifdef NIGHTERM_MALLOC_IS_AVAILABLE
    // TODO: Free memory for backbuffer
#endif
    return status;
  }

  config->rows = (config->fb_height / config->font_header.height);
  config->cols = (config->fb_width / config->font_header.width);
  config->cur_x = 0;
  config->cur_y = 0;
  config->fg_color = 0xFFFFFFFF;
  config->bg_color = 0xFF000000;

  return NIGHTERM_SUCCESS;
}

/**
 * @brief Frees all allocated memory and deinitializes Nighterm.
 *
 *
 * @param          context
 *                 Nighterm context
 */
void
nighterm_shutdown(struct nighterm_ctx *context)
{
  context->fb_addr = 0;
  context->fb_width = 0;
  context->fb_height = 0;
  context->fb_pitch = 0;
  context->fb_bpp = 0;
  context->rows = 0;
  context->cols = 0;
  context->cur_x = 0;
  context->cur_y = 0;
  context->fg_color = 0;
  context->bg_color = 0;
  context->font_header = (psf2_header){0};
  context->font_data = NULL;
#ifdef NIGHTERM_MALLOC_IS_AVAILABLE
  context->free(context->backbuffer);
  context->malloc = NULL;
  context->free = NULL;
#endif
  context->malloc = NULL;
  context->free = NULL;
}

/**
 * @brief Changes the font.
 *
 * @param context
 *        Pointer to Nighterm context
 *
 * @param font
 *        Pointer to a buffer containing a new PSF2 font
 *
 * @return NIGHTERM_SUCCESS if the font has been changed sucessfully;
 *         NIGHTERM_INVALID_FONT otherwise.
 */
int
nighterm_set_font(struct nighterm_ctx *context, void *font)
{
  if (font == NULL) {
    return -1;
  }

  return nighterm_priv___parse_psf2_font(font, &context->font_header, &context->font_data);
}

/**
 * @brief Sets foreground (text) color for the currently selected terminal.
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          r
 *                 Red color value (0-255)
 *
 * @param          g
 *                 Green color value (0-255)
 *
 * @param          b
 *                 Blue color value (0-255)
 */
void
nighterm_set_fg_color(struct nighterm_ctx *context, uint8_t r, uint8_t g, uint8_t b)
{
  context->fg_color =
    (0xFF << 24) | (r << 16) | (g << 8) | b;
}

/**
 * @brief Sets background color for the currently selected terminal.
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          r
 *                 Red color value (0-255)
 *
 * @param          g
 *                 Green color value (0-255)
 *
 * @param          b
 *                 Blue color value (0-255)
 */
void
nighterm_set_bg_color(struct nighterm_ctx *context, uint8_t r, uint8_t g, uint8_t b)
{
  context->bg_color =
    (0xFF << 24) | (r << 16) | (g << 8) | b;
}

/**
 * @brief Sets the cursor to an absolute position.
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          x
 *                 Absolute x position
 *
 * @param          y
 *                 Absolute y position
 *
 * @todo Check if the coordinates exceed the framebuffer's width/height
 */
void
nighterm_set_cursor_position(struct nighterm_ctx *context, uint32_t x, uint32_t y)
{
  context->cur_x = x;
  context->cur_y = y;
}

/**
 * @brief Sets the cursor to a relative position from its current position.
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          x
 *                 Relative x position
 *
 * @param          y
 *                 Relative y position
 *
 * @todo Check if the coordinates exceed the framebuffer's width/height
 */
void
nighterm_move_cursor(struct nighterm_ctx *context, int32_t x, int32_t y)
{
  context->cur_x += x;
  context->cur_y += y;
}

/**
 * @brief Gets the current cursor's coordinates
 *
 * @param          context
 *                 Nighterm context
 *
 * @param ptr      x
 *                 X position
 *
 * @param ptr      y
 *                 Y position
 */
void
nighterm_get_cursor_position(struct nighterm_ctx *context, uint32_t* x, uint32_t* y)
{
  *x = context->cur_x;
  *y = context->cur_y;
}

/**
 * @brief Clears the screen with a single color
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          r
 *                 Red color value (0-255)
 *
 * @param          g
 *                 Green color value (0-255)
 *
 * @param          b
 *                 Blue color value (0-255)
 */
void
nighterm_flush(struct nighterm_ctx *context, uint8_t r, uint8_t g, uint8_t b)
{
  for (uint64_t y = 0; y < context->fb_height; y++) {
    for (uint64_t x = 0; x < context->fb_width; x++) {
      nighterm_priv___putpixel(context, x, y, r, g, b);
    }
  }
}

/**
 * @brief Draws a single character using the currently selected font.
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          row
 *                 Character's row
 *
 * @param          col
 *                 Character's column
 *
 * @param          c
 *                 Character to be drawn
 */
void
nighterm_render_char(struct nighterm_ctx *context, int row, int col, char c)
{
  int rounding =
    ((context->font_header.width % 8) != 0) ^
    (context->font_header.width == 9);
  uint8_t* glyph =
    context->font_data +
    c * context->font_header.charSize;

  for (size_t y = 0;
       y < context->font_header.height;
       y++) {
    for (size_t x = 0;
         x < context->font_header.width;
         x++) {
      if ((glyph[y * ((context->font_header.width /
                       8) +
                      rounding) +
                 x / 8] >>
           (7 - x % 8)) &
          1) {
        uint8_t r =
          (uint8_t)(context->fg_color >> 16) &
          0xFF;
        uint8_t g =
          (uint8_t)(context->fg_color >> 8) &
          0xFF;
        uint8_t b = (uint8_t)context->fg_color;
        nighterm_priv___putpixel(context,
          col * context->font_header.width + x,
          row * context->font_header.height +
            y,
          r,
          g,
          b);
      } else {
        uint8_t r =
          (uint8_t)(context->bg_color >> 16) &
          0xFF;
        uint8_t g =
          (uint8_t)(context->bg_color >> 8) &
          0xFF;
        uint8_t b = (uint8_t)context->bg_color;
        nighterm_priv___putpixel(context,
          col * context->font_header.width + x,
          row * context->font_header.height +
            y,
          r,
          g,
          b);
      }
    }
  }
}

/**
 * @brief Writes the backbuffer's contents to the framebuffer
 *
 * @param          context
 *                 Nighterm context
 */
void nighterm_swap_buffers(struct nighterm_ctx *context)
{
  memcpy(context->fb_addr, context->backbuffer, context->fb_height * context->fb_pitch);
}

/**
 * @brief Parses a single character for escape sequences and draws it
 *
 * @param          context
 *                 Nighterm context
 *
 * @param          c
 *                 Character to be drawn
 */
void
nighterm_write(struct nighterm_ctx *context, char c)
{
  switch (c) {
    case '\n':
      context->cur_x = 0;
      context->cur_y++;
      break;
    case '\t':
      context->cur_x += NIGHTERM_INDENT_WIDTH;
      break;
    case '\b':
      nighterm_render_char(context, context->cur_y,
                           context->cur_x,
                           ' ');

      // this should be handled better
      // for now, try not to use \b.
      context->cur_x -= 1;
      break;
    case 0:
      break; // ignore termination
    default:
      // Why does this not work???
      if (context->cur_x >=
          context->cols) {
        context->cur_x = 0;
        context->cur_y++;
      }
      nighterm_render_char(context, context->cur_y,
                           context->cur_x,
                           c);
      context->cur_x++;
      break;
  }

  nighterm_swap_buffers(context);
}
