#include "color.h"

unsigned long long encode(ColorCell cell) {
    unsigned long long result = 0;

    result |= ((unsigned long long)cell.ascii << 48);
    result |= ((unsigned long long)cell.fgColor.red << 40);
    result |= ((unsigned long long)cell.fgColor.green << 32);
    result |= ((unsigned long long)cell.fgColor.blue << 24);
    result |= ((unsigned long long)cell.bgColor.red << 16);
    result |= ((unsigned long long)cell.bgColor.green << 8);
    result |= cell.bgColor.blue;

    return result;
}

ColorCell decode(unsigned long long combined) {
    ColorCell cell;
    cell.ascii = (combined >> 48) & 0xFF;
    cell.fgColor.red = (combined >> 40) & 0xFF;
    cell.fgColor.green = (combined >> 32) & 0xFF;
    cell.fgColor.blue = (combined >> 24) & 0xFF;
    cell.bgColor.red = (combined >> 16) & 0xFF;
    cell.bgColor.green = (combined >> 8) & 0xFF;
    cell.bgColor.blue = combined & 0xFF;
    return cell;
}
