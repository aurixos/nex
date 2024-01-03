#ifndef COLOR_H
#define COLOR_H

typedef struct {
    unsigned char ascii;
    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    } fgColor;
    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    } bgColor;
} ColorCell;

unsigned long long encode(ColorCell cell);
ColorCell decode(unsigned long long combined);

#endif // COLOR_H