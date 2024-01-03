#include "nighterm.h"

static struct nighterm_fbinfo fbinfo = {0};

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
int init_nighterm(void *font,
                void *framebuffer_addr,
                uint64_t framebuffer_width,
                uint64_t framebuffer_height,
                uint64_t framebuffer_pitch,
                uint16_t framebuffer_bpp)
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
    
    char *psf2buf = font;
    psf2Hdr hdr = *(psf2Hdr *)font;
    psf2buf += hdr.headerSize;

    if (PSF_MODE == 2 || PSF_MODE == 1 ? (hdr.magic[0] != PSF_MAGIC0 || hdr.magic[1] != PSF_MAGIC1 || hdr.magic[2] != PSF_MAGIC2 || hdr.magic[3] != PSF_MAGIC3) : 0)
        return NIGHTERM_FONT_INVALID;

    size_t buffer_size = (size_t)(getScreenWidth() / hdr.width) * (getScreenHeight() / hdr.height);
    term.fonthdr = hdr;
    term.fontData = psf2buf;
    term.rows = (getScreenHeight() / hdr.height);
    term.cols = (getScreenWidth() / hdr.width);
    term.curX = 0;
    term.curY = 0;
    term.title = "Nighterm";

    fbinfo.addr = framebuffer_addr;
    fbinfo.width = framebuffer_width;
    fbinfo.height = framebuffer_height;
    fbinfo.pitch = framebuffer_pitch;
    fbinfo.bpp = framebuffer_bpp;

    nighterm_clear();
    nighterm_do_curinv();

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
    int rounding = ((term.fonthdr.width % 8) != 0) ^ (term.fonthdr.width == 9);
    uint8_t *glyph = term.fontData + ch * term.fonthdr.charSize;

    for (size_t y = 0; y < term.fonthdr.height; y++)
    {
        for (size_t x = 0; x < term.fonthdr.width; x++)
        {
            if ((glyph[y * ((term.fonthdr.width / 8) + rounding) + x / 8] >> (7 - x % 8)) & 1)
            {
                draw_pixel(col * term.fonthdr.width + x, row * term.fonthdr.height + y, fg_r, fg_g, fg_b);
            }
            else
            {
                draw_pixel(col * term.fonthdr.width + x, row * term.fonthdr.height + y, bg_r, bg_g, bg_b);
            }
        }
    }
}

void nighterm_refresh()
{
    // Note: do not overuse this function since refreshing one character at a time is much mre efficient
    term.curX = 0;
    term.curY = 0;
    int row, col;
    for (row = 0; row < term.rows; row++)
    {
        for (col = 0; col < term.cols; col++)
        {
            ColorCell cell = term.buffer[row * term.cols + col];
            nighterm_render_char(row, col, cell.ascii);
        }
    }
}

void nighterm_clear()
{
    size_t buffer_size = (size_t)term.rows * term.cols;
    memset(term.buffer, ' ', buffer_size);
    nighterm_refresh();
}

void nighterm_write(char ch)
{
    size_t buffer_size = (size_t)term.rows * term.cols;
    nighterm_redraw();


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
        term.curX = 0;
        term.curY++;
        break;
    case '\t':
        term.curX += INDENT_AMOUNT - (term.curX % INDENT_AMOUNT);
        break;
    case '\b':
        term.curX -= 1;
        break;
    case 0:
        nighterm_do_curinv();
        break; // ignore termination
    default:
        int bufferIndex = term.curY * term.cols + term.curX;
        term.buffer[bufferIndex].ascii = ch;
        nighterm_render_char(term.curY, term.curX, ch);
        term.curX++;
        if (term.curX - 1 == term.cols)
        {
            term.curY++;
        }
        nighterm_do_curinv();
        break;
    }
}

void nighterm_move_cursor(int row, int col)
{
    nighterm_redraw();
    term.curX = col;
    term.curY = row;
    nighterm_do_curinv();
}
void nighterm_redraw()
{
    int bufferIndex = term.curY * term.cols + term.curX;
    nighterm_render_char(term.curY, term.curX, term.buffer[bufferIndex].ascii);
}

void nighterm_do_curinv()
{
    uint8_t tmp_r = 0;
    uint8_t tmp_g = 0;
    uint8_t tmp_b = 0;

    tmp_r = bg_r;
    tmp_g = bg_g;
    tmp_b = bg_b;

    bg_r = fg_r;
    bg_g = fg_g;
    bg_b = fg_b;

    fg_r = tmp_r;
    fg_g = tmp_g;
    fg_b = tmp_b;

    nighterm_redraw();

    tmp_r = bg_r;
    tmp_g = bg_g;
    tmp_b = bg_b;

    bg_r = fg_r;
    bg_g = fg_g;
    bg_b = fg_b;

    fg_r = tmp_r;
    fg_g = tmp_g;
    fg_b = tmp_b;
}