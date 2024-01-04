# Nighterm
An awesome terminal emulator 

## Usage

Initializing Nighterm is as simple as calling `init_nighterm()`!

Limine example:
```c
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 1
};

static struct limine_framebuffer framebuffer_response;

/* initialization */

framebuffer_response = framebuffer_request.response;

nighterm_init(psf2_buffer,
                framebuffer_request->address,
                framebuffer_request->width,
                framebuffer_request->height,
                framebuffer_request->pitch,
                framebuffer_request->bpp);
```

It might be desirable to check for the return value of the initialization function to make sure Nighterm is ready to be used.
You can find a list of possible return codes in [nighterm.h](nighterm.h).

### TODO

- If `NULL` is passed into the font parameter, use a builtin font instead.

# Credits

Nighterm Extended is a fork of [Nighterm](https://github.com/KevinAlavik/Nighterm) written (and maintained) by [puffer](https://github.com/KevinAlavik).

This software is released under the GPLv3 license.
