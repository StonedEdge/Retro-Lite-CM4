#ifndef KEYCODES_H_ 
#define KEYCODES_H_

#define esc 27
#define backspace 8
#define enter 13
#define right 14
#define left 15


char osKeyboardLowercase[5][10] = {
    {esc, left, right, 32, 45, 47, enter, 0, 0, 0},
    {0, 122, 120, 99, 118, 98, 110, 109, 44, backspace},
    {97, 115, 100, 102, 103, 104, 106, 107, 108, 46},
    {113, 119, 101, 114, 116, 121, 117, 105, 111, 112},
    {49, 50, 51, 52, 53, 54, 55, 56, 57, 48}
};
char osKeyboardUppercase[5][10] = {
    {esc, left, right, 32, 95, 63, enter, 0, 0, 0},
    {0, 90, 88, 67, 86, 66, 78, 77, 60, backspace},
    {65, 83, 68, 70, 71, 72, 74, 75, 76, 62},
    {81, 87, 69, 82, 84, 89, 85, 73, 79, 80},
    {33, 64, 35, 36, 37, 94, 38, 42, 40, 41}
};

#endif