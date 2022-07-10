/* *************DOC***************
 * pieces_tex : array of textures, one per chess piece
 * pieces_col : array of col coordinates, one per chess piece
 * pieces_row : array of row coordinates, one per chess piece
 * *******************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "window_info.h"
#include "text.h"                                               // No text yet
#include <assert.h>
#include "calc.h"
#include "piece.h"

int main(int argc, char *argv[])
{
    for(int i=0; i<argc; i++) puts(argv[i]);

    // Setup
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
    SDL_Texture *pieces_tex[NUM_PIECES];                        // Array of textures
    int pieces_col[NUM_PIECES];                                 // Array of col coord
    int pieces_row[NUM_PIECES];                                 // Array of row coord
    /* bool pieces_drag[NUM_PIECES];                               // Array of mouse-drag state */
    for(int i=0; i<16; i++)                                     // TODO: change to 32
    {
        char *file; SDL_Color color; int col; int row; // bool drag;
        switch(i)
        {
            case BLACK_PAWN_A: file="pawn.txt"; color=black; col=0; row=1; /* drag=false; */ break;
            case BLACK_PAWN_B: file="pawn.txt"; color=black; col=1; row=1; /* drag=false; */ break;
            case BLACK_PAWN_C: file="pawn.txt"; color=black; col=2; row=1; /* drag=false; */ break;
            case BLACK_PAWN_D: file="pawn.txt"; color=black; col=3; row=1; /* drag=false; */ break;
            case BLACK_PAWN_E: file="pawn.txt"; color=black; col=4; row=1; /* drag=false; */ break;
            case BLACK_PAWN_F: file="pawn.txt"; color=black; col=5; row=1; /* drag=false; */ break;
            case BLACK_PAWN_G: file="pawn.txt"; color=black; col=6; row=1; /* drag=false; */ break;
            case BLACK_PAWN_H: file="pawn.txt"; color=black; col=7; row=1; /* drag=false; */ break;
            case WHITE_PAWN_A: file="pawn.txt"; color=white; col=0; row=6; /* drag=false; */ break;
            case WHITE_PAWN_B: file="pawn.txt"; color=white; col=1; row=6; /* drag=false; */ break;
            case WHITE_PAWN_C: file="pawn.txt"; color=white; col=2; row=6; /* drag=false; */ break;
            case WHITE_PAWN_D: file="pawn.txt"; color=white; col=3; row=6; /* drag=false; */ break;
            case WHITE_PAWN_E: file="pawn.txt"; color=white; col=4; row=6; /* drag=false; */ break;
            case WHITE_PAWN_F: file="pawn.txt"; color=white; col=5; row=6; /* drag=false; */ break;
            case WHITE_PAWN_G: file="pawn.txt"; color=white; col=6; row=6; /* drag=false; */ break;
            case WHITE_PAWN_H: file="pawn.txt"; color=white; col=7; row=6; /* drag=false; */ break;
            default: file="bad"; color=(SDL_Color){0,0,0,0}; col=-1;row=-1;/* drag=false; */ break;
        }
        /* printf("i: %d, file: %s\n", i, file); fflush(stdout); */
        // pieces_tex[BLACK_PAWN_A] = NULL; piece_load_art(ren, &pieces_tex[BLACK_PAWN_A], "pawn.txt", black);
        pieces_tex[i] = NULL; piece_load_art(ren, &(pieces_tex[i]), file, color);
        pieces_col[i] = col; pieces_row[i] = row; // pieces_drag[i] = drag;
    }
    SDL_Texture *BBknight_tex = NULL; piece_load_art(ren, &BBknight_tex, "knight.txt", black);// Load black knight art
    SDL_Texture *WBknight_tex = NULL; piece_load_art(ren, &WBknight_tex, "knight.txt", white);// Load white knight art
    SDL_Texture *BGknight_tex = NULL; piece_load_art(ren, &BGknight_tex, "knight.txt", black);// Load black knight art
    SDL_Texture *WGknight_tex = NULL; piece_load_art(ren, &WGknight_tex, "knight.txt", white);// Load white knight art
    SDL_Texture *BCbishop_tex = NULL; piece_load_art(ren, &BCbishop_tex, "bishop.txt", black);// Load black bishop art
    SDL_Texture *WCbishop_tex = NULL; piece_load_art(ren, &WCbishop_tex, "bishop.txt", white);// Load white bishop art
    SDL_Texture *BFbishop_tex = NULL; piece_load_art(ren, &BFbishop_tex, "bishop.txt", black);// Load black bishop art
    SDL_Texture *WFbishop_tex = NULL; piece_load_art(ren, &WFbishop_tex, "bishop.txt", white);// Load white bishop art
    SDL_Texture *BArook_tex = NULL; piece_load_art(ren, &BArook_tex, "rook.txt", black);// Load black rook art
    SDL_Texture *WArook_tex = NULL; piece_load_art(ren, &WArook_tex, "rook.txt", white);// Load white rook art
    SDL_Texture *BHrook_tex = NULL; piece_load_art(ren, &BHrook_tex, "rook.txt", black);// Load black rook art
    SDL_Texture *WHrook_tex = NULL; piece_load_art(ren, &WHrook_tex, "rook.txt", white);// Load white rook art
    SDL_Texture *Bqueen_tex = NULL; piece_load_art(ren, &Bqueen_tex, "queen.txt", black);// Load black queen art
    SDL_Texture *Wqueen_tex = NULL; piece_load_art(ren, &Wqueen_tex, "queen.txt", white);// Load white queen art
    SDL_Texture *Bking_tex = NULL; piece_load_art(ren, &Bking_tex, "king.txt", black);// Load black king art
    SDL_Texture *Wking_tex = NULL; piece_load_art(ren, &Wking_tex, "king.txt", white);// Load white king art
    // The piece we are working on the art for (drawn at A3 A4 in white and A5 A6 in black):
    SDL_Texture *Bpiece_tex  = NULL; piece_load_art(ren, &Bpiece_tex, "piece.txt", black);  // Load temp piece art
    SDL_Texture *Wpiece_tex  = NULL; piece_load_art(ren, &Wpiece_tex, "piece.txt", white);  // Load temp piece art

    // Game state
    bool quit = false;
    SDL_Rect mouse_tile;
    bool mouse_down = false;
    // State of the active piece
    enum piece_name ActivePiece_name = BLACK_PAWN_A;                   // Active piece
    bool ActivePiece_drag = false;                                    // Active piece dragged by mouse?

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
                            piece_load_art(ren, &Bpiece_tex, "piece.txt", black);
                            piece_load_art(ren, &Wpiece_tex, "piece.txt", white);
                            break;
                        default: break;
                    }
                }
                if(e.type == SDL_MOUSEBUTTONDOWN)
                {
                    mouse_down = true;
                }
                if(e.type == SDL_MOUSEBUTTONUP)
                {
                    mouse_down = false;
                    ActivePiece_drag = false;
                }
            }
        }
        { // Filtered
            SDL_PumpEvents();                                   // Update event queue
            const Uint8 *k = SDL_GetKeyboardState(NULL);        // Scan keys
            if(  k[SDL_SCANCODE_Q]  ) quit = true;              // q to quit
        }
        // Mouse: mouse_down
        { // Update state of ActivePiece
            if(mouse_down)
            {
                int tile_dim = calc_tile_dim(wI.w, wI.h);
                int x,y;                                        // Mouse x,y
                { // Green square
                    SDL_GetMouseState(&x, &y);
                    // Snap mouse xy to chessboard square
                    calc_snap(&x, &y, wI.w, wI.h);
                    // Mouse tile is the green square
                    mouse_tile.x=x;
                    mouse_tile.y=y;
                    mouse_tile.w=tile_dim;
                    mouse_tile.h=tile_dim;
                }
                int col, row;
                { // Convert mouse xy to chessboard col,row coordinates
                    SDL_Rect border = calc_border(wI.w, wI.h);
                    col = (int)(mouse_tile.x - border.x)/tile_dim;
                    row = (int)(mouse_tile.y - border.y)/tile_dim;
                }
                if(ActivePiece_drag == true)
                { // If dragging a piece, follow the mouse;
                    // I want to do this, but I need to change my render function
                    /* pieces_col[ActivePiece_name] = x; */
                    /* pieces_row[ActivePiece_name] = y; */
                    // Do this instead for now
                    pieces_col[ActivePiece_name] = col;
                    pieces_row[ActivePiece_name] = row;
                }
                else
                { // If mousedown on a piece, pick it up
                    // If a piece is at the mousedown col,row start dragging it
                    for(int i=0; i<NUM_PIECES; i++)
                    {
                        if((  pieces_col[i] == col  ) && (  pieces_row[i] == row  ))
                        {
                            /* pieces_drag[i] = true; */
                            // Change the active piece
                            ActivePiece_drag = true;
                            ActivePiece_name = i;
                            break;
                        }
                    }
                }
            }
        }

        // Render
        { // Grey background
            SDL_Color grey = {.r=50, .g=50, .b=50, .a=0xFF};
            SDL_SetRenderDrawColor(ren, grey.r, grey.g, grey.b, grey.a);
            SDL_RenderClear(ren);
        }
        // Chess board border
        SDL_Rect border = calc_border(wI.w, wI.h);
        int tile_dim = calc_tile_dim(wI.w, wI.h);
        { // Chess board
            // Dark squares (saddle brown)
            SDL_Color sbrown = {.r=139, .g=69, .b=19, .a=0xFF};
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
                    SDL_Rect tile = { .x=border.x+tile_dim*(2*f+k),
                                      .y=border.y+(r*tile_dim),
                                      .w=tile_dim,
                                      .h=tile_dim
                                    };
                    SDL_RenderFillRect(ren, &tile);
                }
            }
        }
        { // Temporary mouse test artwork for click happens good
            if(mouse_down)
            {
                SDL_SetRenderDrawColor(ren, 0, 0xFF, 0, 0x3F);
                SDL_RenderFillRect(ren, &mouse_tile);
            }
        }
        { // Pieces
            // Make rectangle for where to draw piece
            /* int piece_dim = (wI.w > wI.h) ? wI.h : wI.w; piece_dim /= 12; */
            int piece_dim = calc_piece_dim(wI.w, wI.h);
            // Pawns
            for(int i=0; i<16; i++)                                     // TODO: change to 32
            {
                piece_render(ren, pieces_tex[i], wI.w, wI.h, pieces_col[i], pieces_row[i]);
            }
            
            // Doubles (knights, bishops, rooks)
            piece_render(ren, BBknight_tex, wI.w, wI.h, 1,0);   // Black knight at b8
            piece_render(ren, WBknight_tex, wI.w, wI.h, 1,7);   // White knight at b1
            piece_render(ren, BGknight_tex, wI.w, wI.h, 6,0);   // Black knight at g8
            piece_render(ren, WGknight_tex, wI.w, wI.h, 6,7);   // White knight at g1

            piece_render(ren, BCbishop_tex, wI.w, wI.h, 2,0);   // Black bishop at c8
            piece_render(ren, WCbishop_tex, wI.w, wI.h, 2,7);   // White bishop at c1
            piece_render(ren, BFbishop_tex, wI.w, wI.h, 5,0);   // Black bishop at f8
            piece_render(ren, WFbishop_tex, wI.w, wI.h, 5,7);   // White bishop at f1

            piece_render(ren, BArook_tex, wI.w, wI.h, 0,0);     // Black rook at a8
            piece_render(ren, WArook_tex, wI.w, wI.h, 0,7);     // White rook at a1
            piece_render(ren, BHrook_tex, wI.w, wI.h, 7,0);     // Black rook at h8
            piece_render(ren, WHrook_tex, wI.w, wI.h, 7,7);     // White rook at h1
            // Singles
            piece_render(ren, Bqueen_tex, wI.w, wI.h, 3,0);     // Black queen at d8
            piece_render(ren, Wqueen_tex, wI.w, wI.h, 3,7);     // White queen at d1
            piece_render(ren, Bking_tex, wI.w, wI.h, 4,0);      // Black king at e8
            piece_render(ren, Wking_tex, wI.w, wI.h, 4,7);      // White king at e1
            { // Temp piece (for working on drawings)
                int piece_x = (tile_dim - piece_dim)/2;             // Center piece x
                int piece_y = (tile_dim - piece_dim)/2;             // Center piece y
                piece_x += border.x; piece_y += border.y;           // Place piece
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
            SDL_Delay(10);
        }
    }

    // Shutdown
    for(int i=0; i<16; i++)
    {
        SDL_DestroyTexture(pieces_tex[i]);
    }

    SDL_DestroyTexture(Bking_tex);
    SDL_DestroyTexture(Wking_tex);
    SDL_DestroyTexture(Bqueen_tex);
    SDL_DestroyTexture(Wqueen_tex);
    SDL_DestroyTexture(BHrook_tex);
    SDL_DestroyTexture(WHrook_tex);
    SDL_DestroyTexture(BArook_tex);
    SDL_DestroyTexture(WArook_tex);
    SDL_DestroyTexture(BCbishop_tex);
    SDL_DestroyTexture(WCbishop_tex);
    SDL_DestroyTexture(BFbishop_tex);
    SDL_DestroyTexture(WFbishop_tex);
    SDL_DestroyTexture(BBknight_tex);
    SDL_DestroyTexture(WBknight_tex);
    SDL_DestroyTexture(BGknight_tex);
    SDL_DestroyTexture(WGknight_tex);
    SDL_DestroyTexture(Bpiece_tex);
    SDL_DestroyTexture(Wpiece_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
