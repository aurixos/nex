# Nighterm Extended
An awesome terminal emulator 

> [!WARNING]
> NEx is **NOT** ready for use yet. A lot of important features are missing, and will not be added since a complete rewrite is to be written.

## Usage

> [!NOTE]  
> For an always up-to-date example of NEx, you can visit the [NEx template](https://github.com/schkwve/nighterm-extended-example) repository.

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

struct nighterm_ctx context;

nighterm_init(context,
                psf2_buffer,
                framebuffer_response->address,
                framebuffer_response->width,
                framebuffer_response->height,
                framebuffer_response->bpp
                kmalloc,
                kfree);
```

`font`, `malloc` and `free` parameters are optional and can be NULL. All other parameters (context and framebuffer information) are required.

You should check for the return value of the initialization function to make sure Nighterm is ready to be used.
You can find a list of possible return codes in [nighterm.h](nighterm.h).

If you wish to supply `kmalloc()` and `kfree()` to NEx, make sure to define `NIGHTERM_MALLOC_IS_AVAILABLE` macro before including `nighterm.h`.
If you supply NULL to the `font` parameter, a default built-in font will be used. The font can then be changed later.

# Credits

Nighterm Extended is a fork of [Nighterm](https://github.com/KevinAlavik/Nighterm) written by [puffer](https://github.com/KevinAlavik).

This software is released under the GPLv3 license.
