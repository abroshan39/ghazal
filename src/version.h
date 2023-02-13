/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 1
#define VERSION_MINOR 5
#define VERSION_PATCH 0
#define VERSION_NAME "کیمیای عشق"
#define COPYRIGHT_Y1 2012
#define COPYRIGHT_Y2 2023

#define STR(x) #x
#define WIN_VER(w, x, y, z) STR(w) "." STR(x) "." STR(y) "." STR(z)
#define WIN_VER_COPYRIGHT(x, y1, y2, z) x STR(y1) "-" STR(y2) z

#endif // VERSION_H
