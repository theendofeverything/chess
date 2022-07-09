#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "window_info.h"
#include "text.h"                                               // No text yet
#include "str.h"                                                // Bitmap from .txt file
#include <assert.h>
#include "chess.h"

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

void snap(int *x, int *y, int win_w, int win_h)
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
void render_piece(SDL_Renderer *ren, SDL_Texture *piece_tex, int win_w, int win_h, int col, int row)
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

void new_render_piece(SDL_Renderer *ren, ChessPiece *chess_piece, int win_w, int win_h)
{
    int col = chess_piece->col;
    int row = chess_piece->row;
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
    SDL_RenderCopy(ren, chess_piece->tex, NULL, &piece_rect);
}

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
    ChessPiece *BlackPawn_a = ChessPiece_new(ren, "pawn.txt", black, 0,1);       // Black pawn a7
    /* ChessPiece *WhitePawn_a = ChessPiece_new(ren, "pawn.txt", white, 0,6);       // White pawn a2 */
    /* ChessPiece *BlackPawn_b = ChessPiece_new(ren, "pawn.txt", black, 1,1);       // Black Pawn b7 */
    /* ChessPiece *WhitePawn_b = ChessPiece_new(ren, "pawn.txt", white, 1,6);       // White pawn b2 */
    /* ChessPiece *BlackPawn_c = ChessPiece_new(ren, "pawn.txt", black, 2,1);       // Black Pawn c7 */
    /* ChessPiece *WhitePawn_c = ChessPiece_new(ren, "pawn.txt", white, 2,6);       // White pawn c2 */
    /* ChessPiece *BlackPawn_d = ChessPiece_new(ren, "pawn.txt", black, 3,1);       // Black Pawn d7 */
    /* ChessPiece *WhitePawn_d = ChessPiece_new(ren, "pawn.txt", white, 3,6);       // White pawn d2 */
    /* ChessPiece *BlackPawn_e = ChessPiece_new(ren, "pawn.txt", black, 4,1);       // Black Pawn e7 */
    /* ChessPiece *WhitePawn_e = ChessPiece_new(ren, "pawn.txt", white, 4,6);       // White pawn e2 */
    /* ChessPiece *BlackPawn_f = ChessPiece_new(ren, "pawn.txt", black, 5,1);       // Black Pawn f7 */
    /* ChessPiece *WhitePawn_f = ChessPiece_new(ren, "pawn.txt", white, 5,6);       // White pawn f2 */
    /* ChessPiece *BlackPawn_g = ChessPiece_new(ren, "pawn.txt", black, 6,1);       // Black Pawn g7 */
    /* ChessPiece *WhitePawn_g = ChessPiece_new(ren, "pawn.txt", white, 6,6);       // White pawn g2 */
    /* ChessPiece *BlackPawn_h = ChessPiece_new(ren, "pawn.txt", black, 7,1);       // Black Pawn h7 */
    /* ChessPiece *WhitePawn_h = ChessPiece_new(ren, "pawn.txt", white, 7,6);       // White pawn h2 */
    /* assert(WhitePawn_h != NULL); */
    SDL_Texture *BApawn_tex   = NULL; load_piece_art(ren, &BApawn_tex, "pawn.txt", black);    //
    SDL_Texture *WApawn_tex   = NULL; load_piece_art(ren, &WApawn_tex, "pawn.txt", white);    //
    SDL_Texture *BBpawn_tex   = NULL; load_piece_art(ren, &BBpawn_tex, "pawn.txt", black);    //
    SDL_Texture *WBpawn_tex   = NULL; load_piece_art(ren, &WBpawn_tex, "pawn.txt", white);    //
    SDL_Texture *BCpawn_tex   = NULL; load_piece_art(ren, &BCpawn_tex, "pawn.txt", black);    //
    SDL_Texture *WCpawn_tex   = NULL; load_piece_art(ren, &WCpawn_tex, "pawn.txt", white);    //
    SDL_Texture *BDpawn_tex   = NULL; load_piece_art(ren, &BDpawn_tex, "pawn.txt", black);    //
    SDL_Texture *WDpawn_tex   = NULL; load_piece_art(ren, &WDpawn_tex, "pawn.txt", white);    //
    SDL_Texture *BEpawn_tex   = NULL; load_piece_art(ren, &BEpawn_tex, "pawn.txt", black);    //
    SDL_Texture *WEpawn_tex   = NULL; load_piece_art(ren, &WEpawn_tex, "pawn.txt", white);    //
    SDL_Texture *BFpawn_tex   = NULL; load_piece_art(ren, &BFpawn_tex, "pawn.txt", black);    //
    SDL_Texture *WFpawn_tex   = NULL; load_piece_art(ren, &WFpawn_tex, "pawn.txt", white);    //
    SDL_Texture *BGpawn_tex   = NULL; load_piece_art(ren, &BGpawn_tex, "pawn.txt", black);    //
    SDL_Texture *WGpawn_tex   = NULL; load_piece_art(ren, &WGpawn_tex, "pawn.txt", white);    //
    SDL_Texture *BHpawn_tex   = NULL; load_piece_art(ren, &BHpawn_tex, "pawn.txt", black);    //
    SDL_Texture *WHpawn_tex   = NULL; load_piece_art(ren, &WHpawn_tex, "pawn.txt", white);    //

    // Doubles
    SDL_Texture *BBknight_tex = NULL; load_piece_art(ren, &BBknight_tex, "knight.txt", black);// Load black knight art
    SDL_Texture *WBknight_tex = NULL; load_piece_art(ren, &WBknight_tex, "knight.txt", white);// Load white knight art
    SDL_Texture *BGknight_tex = NULL; load_piece_art(ren, &BGknight_tex, "knight.txt", black);// Load black knight art
    SDL_Texture *WGknight_tex = NULL; load_piece_art(ren, &WGknight_tex, "knight.txt", white);// Load white knight art
    SDL_Texture *BCbishop_tex = NULL; load_piece_art(ren, &BCbishop_tex, "bishop.txt", black);// Load black bishop art
    SDL_Texture *WCbishop_tex = NULL; load_piece_art(ren, &WCbishop_tex, "bishop.txt", white);// Load white bishop art
    SDL_Texture *BFbishop_tex = NULL; load_piece_art(ren, &BFbishop_tex, "bishop.txt", black);// Load black bishop art
    SDL_Texture *WFbishop_tex = NULL; load_piece_art(ren, &WFbishop_tex, "bishop.txt", white);// Load white bishop art
    SDL_Texture *BArook_tex = NULL; load_piece_art(ren, &BArook_tex, "rook.txt", black);// Load black rook art
    SDL_Texture *WArook_tex = NULL; load_piece_art(ren, &WArook_tex, "rook.txt", white);// Load white rook art
    SDL_Texture *BHrook_tex = NULL; load_piece_art(ren, &BHrook_tex, "rook.txt", black);// Load black rook art
    SDL_Texture *WHrook_tex = NULL; load_piece_art(ren, &WHrook_tex, "rook.txt", white);// Load white rook art
    // Singles
    SDL_Texture *Bqueen_tex = NULL; load_piece_art(ren, &Bqueen_tex, "queen.txt", black);// Load black queen art
    SDL_Texture *Wqueen_tex = NULL; load_piece_art(ren, &Wqueen_tex, "queen.txt", white);// Load white queen art
    SDL_Texture *Bking_tex = NULL; load_piece_art(ren, &Bking_tex, "king.txt", black);// Load black king art
    SDL_Texture *Wking_tex = NULL; load_piece_art(ren, &Wking_tex, "king.txt", white);// Load white king art
    // The piece we are working on the art for (drawn at A3 A4 in white and A5 A6 in black):
    SDL_Texture *Bpiece_tex  = NULL; load_piece_art(ren, &Bpiece_tex, "piece.txt", black);  // Load temp piece art
    SDL_Texture *Wpiece_tex  = NULL; load_piece_art(ren, &Wpiece_tex, "piece.txt", white);  // Load temp piece art

    // Game state
    bool quit = false;
    SDL_Rect mouse_tile;
    bool mouse_down = false;

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
                            load_piece_art(ren, &Bpiece_tex, "piece.txt", black);
                            load_piece_art(ren, &Wpiece_tex, "piece.txt", white);
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
                }
            }
        }
        { // Filtered
            SDL_PumpEvents();                                   // Update event queue
            const Uint8 *k = SDL_GetKeyboardState(NULL);        // Scan keys
            if(  k[SDL_SCANCODE_Q]  ) quit = true;              // q to quit
        }
        if(mouse_down)
        { // Click-dragging
            int x,y;
            SDL_GetMouseState(&x, &y);
            // Convert mouse xy to chessboard square
            snap(&x, &y, wI.w, wI.h);
            mouse_tile.x=x;
            mouse_tile.y=y;
            int tile_dim = calc_tile_dim(wI.w, wI.h);
            mouse_tile.w=tile_dim;
            mouse_tile.h=tile_dim;
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
                SDL_SetRenderDrawColor(ren, 0, 0xFF, 0, 0xFF);
                SDL_RenderFillRect(ren, &mouse_tile);
            }
        }
        { // Pieces
            // Pawns
            /* new_render_piece(ren, BlackPawn_a, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_a, wI.w, wI.h); */
            /* new_render_piece(ren, BlackPawn_b, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_b, wI.w, wI.h); */
            /* new_render_piece(ren, BlackPawn_c, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_c, wI.w, wI.h); */
            /* new_render_piece(ren, BlackPawn_d, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_d, wI.w, wI.h); */
            /* new_render_piece(ren, BlackPawn_e, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_e, wI.w, wI.h); */
            /* new_render_piece(ren, BlackPawn_f, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_f, wI.w, wI.h); */
            /* new_render_piece(ren, BlackPawn_g, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_g, wI.w, wI.h); */
            /* new_render_piece(ren, BlackPawn_h, wI.w, wI.h); */
            /* new_render_piece(ren, WhitePawn_h, wI.w, wI.h); */
            // Pawns
            render_piece(ren, BApawn_tex, wI.w, wI.h, 0,1);     // Black pawn at a7
            render_piece(ren, WApawn_tex, wI.w, wI.h, 0,6);     // White pawn at a2
            render_piece(ren, BBpawn_tex, wI.w, wI.h, 1,1);     // Black pawn at b7
            render_piece(ren, WBpawn_tex, wI.w, wI.h, 1,6);     // White pawn at b2
            render_piece(ren, BCpawn_tex, wI.w, wI.h, 2,1);     // Black pawn at c7
            render_piece(ren, WCpawn_tex, wI.w, wI.h, 2,6);     // White pawn at c2
            render_piece(ren, BDpawn_tex, wI.w, wI.h, 3,1);     // Black pawn at d7
            render_piece(ren, WDpawn_tex, wI.w, wI.h, 3,6);     // White pawn at d2
            render_piece(ren, BEpawn_tex, wI.w, wI.h, 4,1);     // Black pawn at e7
            render_piece(ren, WEpawn_tex, wI.w, wI.h, 4,6);     // White pawn at e2
            render_piece(ren, BFpawn_tex, wI.w, wI.h, 5,1);     // Black pawn at f7
            render_piece(ren, WFpawn_tex, wI.w, wI.h, 5,6);     // White pawn at f2
            render_piece(ren, BGpawn_tex, wI.w, wI.h, 6,1);     // Black pawn at g7
            render_piece(ren, WGpawn_tex, wI.w, wI.h, 6,6);     // White pawn at g2
            render_piece(ren, BHpawn_tex, wI.w, wI.h, 7,1);     // Black pawn at h7
            render_piece(ren, WHpawn_tex, wI.w, wI.h, 7,6);     // White pawn at h2

            // Doubles (knights, bishops, rooks)
            render_piece(ren, BBknight_tex, wI.w, wI.h, 1,0);   // Black knight at b8
            render_piece(ren, WBknight_tex, wI.w, wI.h, 1,7);   // White knight at b1
            render_piece(ren, BGknight_tex, wI.w, wI.h, 6,0);   // Black knight at g8
            render_piece(ren, WGknight_tex, wI.w, wI.h, 6,7);   // White knight at g1

            render_piece(ren, BCbishop_tex, wI.w, wI.h, 2,0);   // Black bishop at c8
            render_piece(ren, WCbishop_tex, wI.w, wI.h, 2,7);   // White bishop at c1
            render_piece(ren, BFbishop_tex, wI.w, wI.h, 5,0);   // Black bishop at f8
            render_piece(ren, WFbishop_tex, wI.w, wI.h, 5,7);   // White bishop at f1

            render_piece(ren, BArook_tex, wI.w, wI.h, 0,0);     // Black rook at a8
            render_piece(ren, WArook_tex, wI.w, wI.h, 0,7);     // White rook at a1
            render_piece(ren, BHrook_tex, wI.w, wI.h, 7,0);     // Black rook at h8
            render_piece(ren, WHrook_tex, wI.w, wI.h, 7,7);     // White rook at h1
            // Singles
            render_piece(ren, Bqueen_tex, wI.w, wI.h, 3,0);     // Black queen at d8
            render_piece(ren, Wqueen_tex, wI.w, wI.h, 3,7);     // White queen at d1
            render_piece(ren, Bking_tex, wI.w, wI.h, 4,0);      // Black king at e8
            render_piece(ren, Wking_tex, wI.w, wI.h, 4,7);      // White king at e1
            { // Temp piece (for working on drawings)
                // Make rectangle for where to draw piece
                int piece_dim = calc_piece_dim(wI.w, wI.h);
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
    SDL_DestroyTexture(BApawn_tex);
    SDL_DestroyTexture(WApawn_tex);
    SDL_DestroyTexture(BBpawn_tex);
    SDL_DestroyTexture(WBpawn_tex);
    SDL_DestroyTexture(BCpawn_tex);
    SDL_DestroyTexture(WCpawn_tex);
    SDL_DestroyTexture(BDpawn_tex);
    SDL_DestroyTexture(WDpawn_tex);
    SDL_DestroyTexture(BEpawn_tex);
    SDL_DestroyTexture(WEpawn_tex);
    SDL_DestroyTexture(BFpawn_tex);
    SDL_DestroyTexture(WFpawn_tex);
    SDL_DestroyTexture(BGpawn_tex);
    SDL_DestroyTexture(WGpawn_tex);
    SDL_DestroyTexture(BHpawn_tex);
    SDL_DestroyTexture(WHpawn_tex);

    ChessPiece_free(BlackPawn_a); BlackPawn_a = NULL;
    /* ChessPiece_free(WhitePawn_a); WhitePawn_a = NULL; */
    /* ChessPiece_free(BlackPawn_b); BlackPawn_b = NULL; */
    /* ChessPiece_free(WhitePawn_b); WhitePawn_b = NULL; */
    /* ChessPiece_free(BlackPawn_c); BlackPawn_c = NULL; */
    /* ChessPiece_free(WhitePawn_c); WhitePawn_c = NULL; */
    /* ChessPiece_free(BlackPawn_d); BlackPawn_d = NULL; */
    /* ChessPiece_free(WhitePawn_d); WhitePawn_d = NULL; */
    /* ChessPiece_free(BlackPawn_e); BlackPawn_e = NULL; */
    /* ChessPiece_free(WhitePawn_e); WhitePawn_e = NULL; */
    /* ChessPiece_free(BlackPawn_f); BlackPawn_f = NULL; */
    /* ChessPiece_free(WhitePawn_f); WhitePawn_f = NULL; */
    /* ChessPiece_free(BlackPawn_g); BlackPawn_g = NULL; */
    /* ChessPiece_free(WhitePawn_g); WhitePawn_g = NULL; */
    /* ChessPiece_free(BlackPawn_h); BlackPawn_h = NULL; */
    /* ChessPiece_free(WhitePawn_h); WhitePawn_h = NULL; */

    SDL_DestroyTexture(Bpiece_tex);
    SDL_DestroyTexture(Wpiece_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}

