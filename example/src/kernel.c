#include <nighterm/nighterm.h>
#include <limine/limine.h>
#include <utilities/printf.h>
#include "idt/idt.h"

#include "libc/mem.h"

#define PSF2_MODE
#define NIGHTERM_LIMINE

static volatile struct limine_module_request mod_request = {
    .id = LIMINE_MODULE_REQUEST, .revision = 0};

int saved_x;
int saved_y;

void _start(void)
{
    init_display();
    init_nighterm(mod_request.response->modules[0]);

    printf("Welcome to the Nighterm example!");
    hlt();
}
