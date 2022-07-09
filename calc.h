#ifndef __CALC_H__
#define __CALC_H__
#include <SDL.h>

int calc_tile_dim(int win_w, int win_h)
{
    int tile_dim = (win_w > win_h) ? win_h : win_w;
    return tile_dim/10;
}

int calc_piece_dim(int win_w, int win_h)
{
    int piece_dim = (win_w > win_h) ? win_h : win_w;
    return piece_dim/12;
}

SDL_Rect calc_border(int win_w, int win_h)
{
    SDL_Rect border;
    border.w = calc_tile_dim(win_w, win_h)*8;
    border.h = border.w;
    border.x = (win_w - border.w)/2;
    border.y = (win_h - border.h)/2;
    return border;
}

void calc_snap(int *x, int *y, int win_w, int win_h)
{
    /* *************DOC***************
     * Snap mouse x,y to chess grid x,y
     * *******************************/
    SDL_Rect border = calc_border(win_w, win_h);
    int tile_dim = calc_tile_dim(win_w, win_h);
    int rel_x = *x - border.x;
    if(rel_x<0) rel_x=0; else if(rel_x>(7*tile_dim)) rel_x = 7*tile_dim; // Clamp
    *x = ((int)(rel_x/tile_dim))*tile_dim + border.x;
    int rel_y = *y - border.y;
    if(rel_y<0) rel_y=0; else if(rel_y>(7*tile_dim)) rel_y = 7*tile_dim; // Clamp
    *y = ((int)(rel_y/tile_dim))*tile_dim + border.y;
}

#endif // __CALC_H__
