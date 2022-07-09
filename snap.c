void snap(int *x, int *y, int win_w, int win_h)
{
    /* *************DOC***************
     * Snap mouse x,y to chess grid x,y
     * *******************************/
    int mouse_x = *x;
    int mouse_y = *y;
    SDL_Rect border = calc_border(win_w, win_h);
    int tile_dim = calc_tile_dim(win_w, win_h);
    // TODO: deal with negative values (mouse outside chess board)
    *x = (mouse_x-border.x)/tile_dim;
}
