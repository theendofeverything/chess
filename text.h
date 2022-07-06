#ifndef __TEXT_H__
#define __TEXT_H__

#include <SDL.h>
#include <SDL_ttf.h>
typedef struct
{
    const char *text;
    SDL_Rect rect;
    SDL_Color fg;
} textbox;

#endif // __TEXT_H__

