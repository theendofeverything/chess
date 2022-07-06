#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "window_info.h"
#include "rand.h"
#include "text.h"
#include "str.h"

void load_piece_art(SDL_Renderer *ren, SDL_Texture **piece_tex)
{
    Str Piece;                                                  // Holds text dwg of chess piece
    { // Copy all characters from piece.txt
        FILE *f = fopen("pawn.txt", "r");
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
    SDL_Surface *surf = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);
    // Color surface pixels based on the piece.txt
    uint32_t *p = surf->pixels;
    for(int r=0; r < surf->h; r++)                              // Walk rows
    {
        for(int c=0; c < surf->w; c++)                          // Walk columns
        {
            // HACK: for now just use color of light tile as background
            // SDL_Color buwood = {.r=222, .g=184, .b=135, .a=0xFF};
            // Color format: aarrggbb
            // Fix: look into FLAGS

            if(*Piece.c == 'x') *p++ = 0x00000000;              // Black
            else if(*Piece.c == '-') *p++ = 0x00DEB887;         // Background
            Piece.c++;
        }
    }

    free(Piece.txt);                                            // Done with string

    // FIX THIS: Cannot create texture over and over
    *piece_tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_SetTextureBlendMode(*piece_tex, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(surf);
}

void update_piece_art(SDL_Renderer *ren, SDL_Texture **piece_tex)
{
    SDL_DestroyTexture(*piece_tex);                             // Destroy existing texture
    load_piece_art(ren, piece_tex);                             // New texture
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

    SDL_Texture *piece_tex; load_piece_art(ren, &piece_tex);    // Load piece art

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
                        case SDLK_SPACE: update_piece_art(ren, &piece_tex); break;
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
        { // White background
            SDL_Color white = {.r=50, .g=50, .b=50, .a=0xFF};
            SDL_SetRenderDrawColor(ren, white.r, white.g, white.b, white.a);
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
            // "White" squares (burly wood)
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
            int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
            int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
            piece_x += border_x; piece_y += border_y;           // Place piece
            SDL_Rect piece_rect = {.x=piece_x, .y=piece_y, .w=piece_dim, .h=piece_dim};
            SDL_RenderCopy(ren, piece_tex, NULL, &piece_rect);
        }
        { // Present to screen
            SDL_RenderPresent(ren);
            SDL_Delay(50);
        }
    }

    // Shutdown
    SDL_DestroyTexture(piece_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
