#include "nighterm.h"

struct nighterm_fbinfo fbinfo = {0};

struct Terminal term;
uint8_t fg_r = 255;
uint8_t fg_g = 255;
uint8_t fg_b = 255;

uint8_t bg_r = 0;
uint8_t bg_g = 0;
uint8_t bg_b = 0;

/**
 * Font points to a PSF2 file buffer
 */
int nighterm_initialize(void *font,
                void *framebuffer_addr,
                uint64_t framebuffer_width,
                uint64_t framebuffer_height,
                uint64_t framebuffer_pitch,
                uint16_t framebuffer_bpp,
                void *(*custom_malloc)(size_t))
{
    if (font == NULL) {
        /* No font supplied. */
        return NIGHTERM_NO_FONT_SUPPLIED;
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

    if (framebuffer_bpp != 32) {
        /* Invalid framebuffer BPP. */
        return NIGHTERM_INVALID_FRAMEBUFFER_BPP;
    }

#ifdef NIGHTERM_MALLOC_IS_AVAILABLE
    if (nighterm_malloc == NULL) {
        return NIGHTERM_MALLOC_IS_NULL;
    }
    nighterm_malloc = custom_malloc;
    term.buffer = nighterm_malloc(framebuffer_height * framebuffer_width * (framebuffer_bpp >> 3));
#else
    (void)custom_malloc;
#endif
    
    char *psf2buf = font;
    psf2Hdr hdr = *(psf2Hdr *)font;
    psf2buf += hdr.headerSize;

    if (PSF_MODE == 2 || PSF_MODE == 1 ? (hdr.magic[0] != PSF_MAGIC0 || hdr.magic[1] != PSF_MAGIC1 || hdr.magic[2] != PSF_MAGIC2 || hdr.magic[3] != PSF_MAGIC3) : 0) {
        return NIGHTERM_FONT_INVALID;
    }

    term.font_header = hdr;
    term.font_data = psf2buf;
    term.rows = (fbinfo.height / hdr.height);
    term.cols = (fbinfo.width / hdr.width);
    term.cx = 0;
    term.cy = 0;
    term.title = "Nighterm";

    fbinfo.addr = framebuffer_addr;
    fbinfo.width = framebuffer_width;
    fbinfo.height = framebuffer_height;
    fbinfo.pitch = framebuffer_pitch;
    fbinfo.bpp = framebuffer_bpp;

    return NIGHTERM_SUCCESS;
}

void nighterm_set_char_fg(uint8_t r, uint8_t b, uint8_t g)
{
    fg_r = r;
    fg_g = g;
    fg_b = b;
}
void nighterm_set_char_bg(uint8_t r, uint8_t b, uint8_t g)
{
    bg_r = r;
    bg_g = g;
    bg_b = b;
}

void nighterm_render_char(int row, int col, char ch)
{
    int rounding = ((term.font_header.width % 8) != 0) ^ (term.font_header.width == 9);
    uint8_t *glyph = term.font_data + ch * term.font_header.charSize;

    for (size_t y = 0; y < term.font_header.height; y++)
    {
        for (size_t x = 0; x < term.font_header.width; x++)
        {
            if ((glyph[y * ((term.font_header.width / 8) + rounding) + x / 8] >> (7 - x % 8)) & 1)
            {
                nighterm_putpixel(col * term.font_header.width + x, row * term.font_header.height + y, fg_r, fg_g, fg_b);
            }
            else
            {
                nighterm_putpixel(col * term.font_header.width + x, row * term.font_header.height + y, bg_r, bg_g, bg_b);
            }
        }
    }
}

void nighterm_write(char ch)
{
    ColorCell cell = {};
    cell.ascii = ch;
    
    cell.fgColor.red = fg_r;
    cell.fgColor.green = fg_g;
    cell.fgColor.blue = fg_b;
    
    cell.bgColor.red = bg_r;
    cell.bgColor.green = bg_g;
    cell.bgColor.blue = bg_b;

    unsigned long long encodedCell = encode(cell);
    ColorCell decodedCell = decode(encodedCell); // Decode the combined value

    // Set foreground and background colors
    nighterm_set_char_fg(decodedCell.fgColor.red, decodedCell.fgColor.green, decodedCell.fgColor.blue);
    nighterm_set_char_bg(decodedCell.bgColor.red, decodedCell.bgColor.green, decodedCell.bgColor.blue);

    switch (ch)
    {
    case '\n':
        term.cx = 0;
        term.cy++;
        break;
    case '\t':
        term.cx += INDENT_AMOUNT - (term.cx % INDENT_AMOUNT);
        break;
    case '\b':
    nighterm_render_char(term.cy, term.cx, ' ');
        term.cx -= 1;
        break;
    case 0:
        break; // ignore termination
    default:
        // Why does this not work???
        // if (term.cx >= term.cols)
        // {
            // term.cx = 0;
            // term.cy++;
        // }
        nighterm_render_char(term.cy, term.cx, ch);
        term.cx++;
        break;
    }
}
