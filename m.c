#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "window_info.h"
#include "rand.h"
#include "text.h"
#include "str.h"

void load_piece_art(SDL_Renderer *ren, SDL_Texture **piece_tex, const char *file, SDL_Color color)
{
    /* *************DOC***************
     * Call load_piece_art once per texture.
     * After that, call update_piece_art.
     * TODO: detect if piece_text is NULL to decide whether or not to destroy
     * texture, then I can get rid of update_piece_art
     * *******************************/
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

void update_piece_art(SDL_Renderer *ren, SDL_Texture **piece_tex, const char *file, SDL_Color color)
{
    SDL_DestroyTexture(*piece_tex);                             // Destroy existing texture
    load_piece_art(ren, piece_tex, file, color);                // New texture
}

int main(int argc, char *argv[])
{
    for(int i=0; i<argc; i++) puts(argv[i]);

    // Setup
    rand_init();                                                // Seed RNG
    SDL_Init(SDL_INIT_VIDEO);
    if(  TTF_Init() <0  )                                       // Init SDL_ttf
    {
        puts("Cannot initialize SDL_ttf");
        SDL_Quit();
        return EXIT_FAILURE;
    }
    WindowInfo wI; WindowInfo_setup(&wI, argc, argv);           // Size and locate window
    SDL_Window *win = SDL_CreateWindow(argv[0], wI.x, wI.y, wI.w, wI.h, wI.flags);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    if(  SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND) <0  ) // Draw with alpha
    {
        puts("Cannot draw with alpha channel");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Color black = {.r=0x11, .g=0x11, .b=0x00, .a=0xFF};
    SDL_Color white = {.r=0xEE, .g=0xEE, .b=0xDD, .a=0xFF};
    SDL_Texture *Bpawn_tex; load_piece_art(ren, &Bpawn_tex, "pawn.txt", black);    // Load pawn art
    SDL_Texture *Wpawn_tex; load_piece_art(ren, &Wpawn_tex, "pawn.txt", white);    // Load pawn art
    SDL_Texture *Bknight_tex; load_piece_art(ren, &Bknight_tex, "knight.txt", black);    // Load pawn art
    SDL_Texture *Wknight_tex; load_piece_art(ren, &Wknight_tex, "knight.txt", white);    // Load pawn art
    SDL_Texture *Bpiece_tex; load_piece_art(ren, &Bpiece_tex, "piece.txt", black); // Load temp piece art
    SDL_Texture *Wpiece_tex; load_piece_art(ren, &Wpiece_tex, "piece.txt", white); // Load temp piece art

    // Game state
    bool quit = false;

    // Game loop
    while(quit == false)
    {
        SDL_GetWindowSize(win, &wI.w, &wI.h);                   // Update window info after resize
        // UI
        { // Polled
            SDL_Event e;
            while(  SDL_PollEvent(&e)  )
            {
                if(e.type == SDL_KEYDOWN)
                {
                    switch(e.key.keysym.sym)
                    {
                        // SPACE to reload artwork
                        case SDLK_SPACE:
                            update_piece_art(ren, &Bpiece_tex, "piece.txt", black);
                            update_piece_art(ren, &Wpiece_tex, "piece.txt", white);
                            break;
                        default: break;
                    }
                }
            }
        }
        { // Filtered
            SDL_PumpEvents();                                   // Update event queue
            const Uint8 *k = SDL_GetKeyboardState(NULL);        // Scan keys
            if(  k[SDL_SCANCODE_Q]  ) quit = true;              // q to quit
        }

        // Render
        { // Grey background
            SDL_Color grey = {.r=50, .g=50, .b=50, .a=0xFF};
            SDL_SetRenderDrawColor(ren, grey.r, grey.g, grey.b, grey.a);
            SDL_RenderClear(ren);
        }
        int tile_dim = (wI.w > wI.h) ? wI.h : wI.w; tile_dim /= 10;
        int border_dim = tile_dim*8;
        int border_x = (wI.w - border_dim)/2;
        int border_y = (wI.h - border_dim)/2;
        { // Chess board
            // Chess board border (and the board dark tile color)
            SDL_Rect border = { .x=border_x,
                                .y=border_y,
                                .w=border_dim,
                                .h=border_dim
                                };
            SDL_Color sbrown = {.r=139, .g=69, .b=19, .a=0xFF}; // Saddle brown
            SDL_SetRenderDrawColor(ren, sbrown.r, sbrown.g, sbrown.b, sbrown.a);
            SDL_RenderFillRect(ren, &border);
            // Light squares (burly wood)
            SDL_Color buwood = {.r=222, .g=184, .b=135, .a=0xFF};
            SDL_SetRenderDrawColor(ren, buwood.r, buwood.g, buwood.b, buwood.a);
            for(int r=0; r<8; r++)                              // Walking ranks (rows)
            {
                for(int f=0; f<4; f++)                          // Walking files (columns)
                {
                    int k = (r%2==0)?0:1;
                    SDL_Rect tile = { .x=border_x+tile_dim*(2*f+k),
                                      .y=border_y+(r*tile_dim),
                                      .w=tile_dim,
                                      .h=tile_dim
                                    };
                    SDL_RenderFillRect(ren, &tile);
                }
            }
        }
        { // Pieces
            // Make rectangle for where to draw piece
            int piece_dim = (wI.w > wI.h) ? wI.h : wI.w; piece_dim /= 12;
            { // Black Pawn
                int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
                int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
                piece_x += border_x; piece_y += border_y;           // Place piece
                piece_y += tile_dim*1;                              // Place temp piece elsewhere
                for(int i=0; i<8; i++)
                {
                    SDL_Rect piece_rect = {.x=piece_x+i*tile_dim, .y=piece_y, .w=piece_dim, .h=piece_dim};
                    SDL_RenderCopy(ren, Bpawn_tex, NULL, &piece_rect);
                }
            }
            { // White Pawn
                int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
                int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
                piece_x += border_x; piece_y += border_y;           // Place piece
                piece_y += tile_dim*6;                              // Place temp piece elsewhere
                for(int i=0; i<8; i++)
                {
                    SDL_Rect piece_rect = {.x=piece_x+i*tile_dim, .y=piece_y, .w=piece_dim, .h=piece_dim};
                    SDL_RenderCopy(ren, Wpawn_tex, NULL, &piece_rect);
                }
            }
            { // Black Knight
                int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
                int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
                piece_x += border_x; piece_y += border_y;           // Place piece
                SDL_Rect piece_rect1 = {.x=piece_x+1*tile_dim, .y=piece_y, .w=piece_dim, .h=piece_dim};
                SDL_RenderCopy(ren, Bknight_tex, NULL, &piece_rect1);
                SDL_Rect piece_rect2 = {.x=piece_x+6*tile_dim, .y=piece_y, .w=piece_dim, .h=piece_dim};
                SDL_RenderCopy(ren, Bknight_tex, NULL, &piece_rect2);
            }
            { // White Knight
                int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
                int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
                piece_x += border_x; piece_y += border_y + tile_dim*7; // Place piece
                SDL_Rect piece_rect1 = {.x=piece_x+1*tile_dim, .y=piece_y, .w=piece_dim, .h=piece_dim};
                SDL_RenderCopy(ren, Wknight_tex, NULL, &piece_rect1);
                SDL_Rect piece_rect2 = {.x=piece_x+6*tile_dim, .y=piece_y, .w=piece_dim, .h=piece_dim};
                SDL_RenderCopy(ren, Wknight_tex, NULL, &piece_rect2);
            }
            { // Temp piece (for working on drawings)
                int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
                int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
                piece_x += border_x; piece_y += border_y;           // Place piece
                piece_y += tile_dim*2;                              // Place temp piece elsewhere
                SDL_Rect piece_rect = {.x=piece_x, .y=piece_y, .w=piece_dim, .h=piece_dim};
                SDL_RenderCopy(ren, Bpiece_tex, NULL, &piece_rect);
                piece_rect.y += tile_dim*1;                              // Place temp piece elsewhere
                SDL_RenderCopy(ren, Bpiece_tex, NULL, &piece_rect);
                piece_rect.y += tile_dim*1;                              // Place temp piece elsewhere
                SDL_RenderCopy(ren, Wpiece_tex, NULL, &piece_rect);
                piece_rect.y += tile_dim*1;                              // Place temp piece elsewhere
                SDL_RenderCopy(ren, Wpiece_tex, NULL, &piece_rect);
            }
        }
        { // Present to screen
            SDL_RenderPresent(ren);
            SDL_Delay(50);
        }
    }

    // Shutdown
    SDL_DestroyTexture(Bknight_tex);
    SDL_DestroyTexture(Wknight_tex);
    SDL_DestroyTexture(Bpawn_tex);
    SDL_DestroyTexture(Wpawn_tex);
    SDL_DestroyTexture(Bpiece_tex);
    SDL_DestroyTexture(Wpiece_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
