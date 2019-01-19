#include <curses.h>
#include "ncursesUtils.h"

int addColor(int textColor, int backgroundColor) {
    static int pair = 3;

    init_pair((short) pair, (short) textColor, (short) backgroundColor);

    return pair++;
}
