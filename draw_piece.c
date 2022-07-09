void draw_piece(SDL_Texture *piece_tex, int win_w, int win_h, int row, int col)
{
    /* *************row, col coordinates***************
     *   0 1 2 3 4 5 6 7
     * 0 w b w b w b w b
     * 1 b w b w b w b w
     * 2 w b w b w b w b
     * 3 b w b w b w b w
     * 4 w b w b w b w b
     * 5 b w b w b w b w
     * 6 w b w b w b w b
     * 7 b w b w b w b w
     * *******************************/
    assert(row >= 0); assert(row < 8);
    assert(col >= 0); assert(col < 8);
    int tile_dim = calc_tile_dim(win_w, win_h);
    int piece_dim = calc_piece_dim(win_w, win_h);
    SDL_Rect border = calc_border(win_w, win_h);
    int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
    int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
    piece_x += border.x; piece_y += border.y;           // Place piece
    SDL_Rect piece_rect = {.x=piece_x+col*tile_dim, .y=piece_y+row*tile_dim, .w=piece_dim, .h=piece_dim};
    SDL_RenderCopy(ren, piece_tex, NULL, &piece_rect);
}
