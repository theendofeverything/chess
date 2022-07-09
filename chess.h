#ifndef __CHESS_H__
#define __CHESS_H__
#include <SDL.h>
#include <stdlib.h>
#include "str.h"

typedef struct
{
    SDL_Texture *tex;
    int col;                                                    // 0 to 7
    int row;                                                    // 0 to 7
    SDL_Color fg;                                               // Foreground color
} ChessPiece;

void load_piece_art(SDL_Renderer *ren, SDL_Texture **piece_tex, const char *file, SDL_Color color)
{
    /* *************DOC***************
     * Call load_piece_art once per texture.
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

ChessPiece* ChessPiece_new(SDL_Renderer *ren, const char *file, SDL_Color fg, int col, int row)
{
    ChessPiece *new_piece = malloc(sizeof(ChessPiece));
    assert(new_piece != NULL);
    new_piece->tex = NULL; load_piece_art(ren, &(new_piece->tex), file, fg);
    assert(new_piece->tex != NULL);
    new_piece->fg = fg;
    new_piece->col = col; new_piece->row = row;
    return new_piece;
}

void ChessPiece_free(ChessPiece *piece)
{
    free(piece->tex);
    piece->tex = NULL;
    free(piece);
}



#endif // __CHESS_H__
