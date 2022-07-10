#ifndef __PIECE_H__
#define __PIECE_H__
#include <SDL.h>
#include "str.h"                                                // Bitmap from .txt file

// Define array size and indices for referring to the 32 chess pieces
#define NUM_PIECES 32
enum piece_name {
    BLACK_PAWN_A=0,
    BLACK_PAWN_B,
    BLACK_PAWN_C,
    BLACK_PAWN_D,
    BLACK_PAWN_E,
    BLACK_PAWN_F,
    BLACK_PAWN_G,
    BLACK_PAWN_H,
    WHITE_PAWN_A,
    WHITE_PAWN_B,
    WHITE_PAWN_C,
    WHITE_PAWN_D,
    WHITE_PAWN_E,
    WHITE_PAWN_F,
    WHITE_PAWN_G,
    WHITE_PAWN_H,
    BLACK_KNIGHT_B,
    BLACK_KNIGHT_G,
    WHITE_KNIGHT_B,
    WHITE_KNIGHT_G,
    BLACK_BISHOP_C,
    BLACK_BISHOP_F,
    WHITE_BISHOP_C,
    WHITE_BISHOP_F,
    BLACK_ROOK_A,
    BLACK_ROOK_H,
    WHITE_ROOK_A,
    WHITE_ROOK_H,
    BLACK_QUEEN,
    WHITE_QUEEN,
    BLACK_KING,
    WHITE_KING
};

void piece_render(SDL_Renderer *ren, SDL_Texture *piece_tex, int win_w, int win_h, int col, int row)
{
    /* *************row, col coordinates***************
     *    COLUMNS
     *    0 1 2 3 4 5 6 7
     *R 0 w b w b w b w b
     *O 1 b w b w b w b w
     *W 2 w b w b w b w b
     *S 3 b w b w b w b w
     *  4 w b w b w b w b
     *  5 b w b w b w b w
     *  6 w b w b w b w b
     *  7 b w b w b w b w
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

void piece_load_art(SDL_Renderer *ren, SDL_Texture **piece_tex, const char *file, SDL_Color color)
{
    /* *************DOC***************
     * Call piece_load_art once per texture.
     * After that, call update_piece_art.
     * If texture is not NULL, this is an existing texture.
     * Destroy this texture before creating a new one.
     * *******************************/
    if (*piece_tex != NULL) SDL_DestroyTexture(*piece_tex);     // Destroy existing texture
    Str Piece;                                                  // Holds text dwg of chess piece
    { // Copy all characters from piece.txt
        FILE *f = fopen(file, "r");
        char c;
        size_t nc = 0; while(  (c=fgetc(f)) != EOF  ) {nc++;}   // Counts chars in file
        rewind(f);
        Piece.txt = malloc((nc+1)*sizeof(char));
        Piece.c = Piece.txt;
        while(  (c=fgetc(f)) != EOF  )
        {
            if(  (c!=' ') && (c!='\n')  ) *Piece.c++ = c;
        }
        Piece.end = Piece.c;
        *Piece.end = '\0';
        Piece.c = Piece.txt;
        fclose(f);
    }

    //                                    flags, w,  h, bit-depth, masks
    SDL_Surface *surf = SDL_CreateRGBSurface(0, 16, 16, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
    // Color surface pixels based on the piece.txt
    // Color format: aarrggbb
    uint32_t c32 = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b ;
    uint32_t *p = surf->pixels;
    for(int r=0; r < surf->h; r++)                              // Walk rows
    {
        for(int c=0; c < surf->w; c++)                          // Walk columns
        {
            if(*Piece.c == 'x') *p++ = c32;                     // Piece fgnd color
            else if(*Piece.c == '-') *p++ = 0x00000000;         // Transparent bgnd
            Piece.c++;
        }
    }

    free(Piece.txt);                                            // Done with string

    *piece_tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_SetTextureBlendMode(*piece_tex, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(surf);
}

#endif // __PIECE_H__
