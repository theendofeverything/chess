/* *************DOC***************
 * Pieces : struct of arrays
 * *******************************/
/* *************TODO***************
 * ~1. Highlight square of last move~
 * ~2. Undo history~
 * 1. Fix promotion bug in undo history: undo after promotion, pawn is gone
 * 2. Only permit legal moves
 * 3. Show art for captured pieces
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

typedef struct
{
    char *name[NUM_PIECES];                                     // Name of each pieces
    SDL_Texture *tex[NUM_PIECES];                               // Texture of each piece
    int col[NUM_PIECES];                                        // Column coordinate of each piece
    int row[NUM_PIECES];                                        // Row coordinate of each piece
    bool color[NUM_PIECES];                                     // bool WHITE or BLACK for each piece
    bool captured[NUM_PIECES];                                  // bool true: captured
} Pieces;

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

    // Setup debug overlay text using SDL_ttf
    const char *font_file = "fonts/ProggyClean.ttf";            // Debug overlay font
    TTF_Font *font = TTF_OpenFont(font_file, 16);               // Get the font
    if(  font == NULL  )
    { // Error handling: NULL probably means font file not found.
        printf("Cannot open font file. Please check \"%s\" exists.", font_file);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    char debug_txt[1024];

    Pieces pieces;

    for(int i=0; i<NUM_PIECES; i++)
    {
        switch(i)
        {
            case BLACK_PAWN_A:  pieces.name[i] = "BLACK_PAWN_A";    break;
            case BLACK_PAWN_B:  pieces.name[i] = "BLACK_PAWN_B";    break;
            case BLACK_PAWN_C:  pieces.name[i] = "BLACK_PAWN_C";    break;
            case BLACK_PAWN_D:  pieces.name[i] = "BLACK_PAWN_D";    break;
            case BLACK_PAWN_E:  pieces.name[i] = "BLACK_PAWN_E";    break;
            case BLACK_PAWN_F:  pieces.name[i] = "BLACK_PAWN_F";    break;
            case BLACK_PAWN_G:  pieces.name[i] = "BLACK_PAWN_G";    break;
            case BLACK_PAWN_H:  pieces.name[i] = "BLACK_PAWN_H";    break;
            case WHITE_PAWN_A:  pieces.name[i] = "WHITE_PAWN_A";    break;
            case WHITE_PAWN_B:  pieces.name[i] = "WHITE_PAWN_B";    break;
            case WHITE_PAWN_C:  pieces.name[i] = "WHITE_PAWN_C";    break;
            case WHITE_PAWN_D:  pieces.name[i] = "WHITE_PAWN_D";    break;
            case WHITE_PAWN_E:  pieces.name[i] = "WHITE_PAWN_E";    break;
            case WHITE_PAWN_F:  pieces.name[i] = "WHITE_PAWN_F";    break;
            case WHITE_PAWN_G:  pieces.name[i] = "WHITE_PAWN_G";    break;
            case WHITE_PAWN_H:  pieces.name[i] = "WHITE_PAWN_H";    break;
            case BLACK_KNIGHT_B:pieces.name[i] = "BLACK_KNIGHT_B";  break;
            case BLACK_KNIGHT_G:pieces.name[i] = "BLACK_KNIGHT_G";  break;
            case WHITE_KNIGHT_B:pieces.name[i] = "WHITE_KNIGHT_B";  break;
            case WHITE_KNIGHT_G:pieces.name[i] = "WHITE_KNIGHT_G";  break;
            case BLACK_BISHOP_C:pieces.name[i] = "BLACK_BISHOP_C";  break;
            case BLACK_BISHOP_F:pieces.name[i] = "BLACK_BISHOP_F";  break;
            case WHITE_BISHOP_C:pieces.name[i] = "WHITE_BISHOP_C";  break;
            case WHITE_BISHOP_F:pieces.name[i] = "WHITE_BISHOP_F";  break;
            case BLACK_ROOK_A:  pieces.name[i] = "BLACK_ROOK_A";    break;
            case BLACK_ROOK_H:  pieces.name[i] = "BLACK_ROOK_H";    break;
            case WHITE_ROOK_A:  pieces.name[i] = "WHITE_ROOK_A";    break;
            case WHITE_ROOK_H:  pieces.name[i] = "WHITE_ROOK_H";    break;
            case BLACK_QUEEN:   pieces.name[i] = "BLACK_QUEEN";     break;
            case WHITE_QUEEN:   pieces.name[i] = "WHITE_QUEEN";     break;
            case BLACK_KING:    pieces.name[i] = "BLACK_KING";      break;
            case WHITE_KING:    pieces.name[i] = "WHITE_KING";      break;
            case NONE:          pieces.name[i] = "NONE";            break;
            default: pieces.name[i] = "ERROR"; break;
        }
    }

    SDL_Color black = {.r=0x11, .g=0x11, .b=0x00, .a=0xFF};
    SDL_Color white = {.r=0xEE, .g=0xEE, .b=0xDD, .a=0xFF};
    for(int i=0; i<NUM_PIECES; i++)
    {
        char *file; SDL_Color color; int col; int row; bool captured;
        switch(i)
        {
            case BLACK_PAWN_A: file="pawn.txt"; color=black; col=0; row=1;     captured=false; break;
            case BLACK_PAWN_B: file="pawn.txt"; color=black; col=1; row=1;     captured=false; break;
            case BLACK_PAWN_C: file="pawn.txt"; color=black; col=2; row=1;     captured=false; break;
            case BLACK_PAWN_D: file="pawn.txt"; color=black; col=3; row=1;     captured=false; break;
            case BLACK_PAWN_E: file="pawn.txt"; color=black; col=4; row=1;     captured=false; break;
            case BLACK_PAWN_F: file="pawn.txt"; color=black; col=5; row=1;     captured=false; break;
            case BLACK_PAWN_G: file="pawn.txt"; color=black; col=6; row=1;     captured=false; break;
            case BLACK_PAWN_H: file="pawn.txt"; color=black; col=7; row=1;     captured=false; break;
            case WHITE_PAWN_A: file="pawn.txt"; color=white; col=0; row=6;     captured=false; break;
            case WHITE_PAWN_B: file="pawn.txt"; color=white; col=1; row=6;     captured=false; break;
            case WHITE_PAWN_C: file="pawn.txt"; color=white; col=2; row=6;     captured=false; break;
            case WHITE_PAWN_D: file="pawn.txt"; color=white; col=3; row=6;     captured=false; break;
            case WHITE_PAWN_E: file="pawn.txt"; color=white; col=4; row=6;     captured=false; break;
            case WHITE_PAWN_F: file="pawn.txt"; color=white; col=5; row=6;     captured=false; break;
            case WHITE_PAWN_G: file="pawn.txt"; color=white; col=6; row=6;     captured=false; break;
            case WHITE_PAWN_H: file="pawn.txt"; color=white; col=7; row=6;     captured=false; break;
            case BLACK_KNIGHT_B: file="knight.txt"; color=black; col=1; row=0; captured=false; break;
            case BLACK_KNIGHT_G: file="knight.txt"; color=black; col=6; row=0; captured=false; break;
            case WHITE_KNIGHT_B: file="knight.txt"; color=white; col=1; row=7; captured=false; break;
            case WHITE_KNIGHT_G: file="knight.txt"; color=white; col=6; row=7; captured=false; break;
            case BLACK_BISHOP_C: file="bishop.txt"; color=black; col=2; row=0; captured=false; break;
            case BLACK_BISHOP_F: file="bishop.txt"; color=black; col=5; row=0; captured=false; break;
            case WHITE_BISHOP_C: file="bishop.txt"; color=white; col=2; row=7; captured=false; break;
            case WHITE_BISHOP_F: file="bishop.txt"; color=white; col=5; row=7; captured=false; break;
            case BLACK_ROOK_A: file="rook.txt"; color=black; col=0; row=0;     captured=false; break;
            case BLACK_ROOK_H: file="rook.txt"; color=black; col=7; row=0;     captured=false; break;
            case WHITE_ROOK_A: file="rook.txt"; color=white; col=0; row=7;     captured=false; break;
            case WHITE_ROOK_H: file="rook.txt"; color=white; col=7; row=7;     captured=false; break;
            case BLACK_QUEEN: file="queen.txt"; color=black; col=3; row=0;     captured=false; break;
            case WHITE_QUEEN: file="queen.txt"; color=white; col=3; row=7;     captured=false; break;
            case BLACK_KING: file="king.txt"; color=black; col=4; row=0;       captured=false; break;
            case WHITE_KING: file="king.txt"; color=white; col=4; row=7;       captured=false; break;
            default: file="bad"; color=(SDL_Color){0,0,0,0}; col=-1;row=-1;    captured=false; break;
        }
        /* printf("i: %d, file: %s\n", i, file); fflush(stdout); */
        // pieces.tex[BLACK_PAWN_A] = NULL; piece_load_art(ren, &pieces.tex[BLACK_PAWN_A], "pawn.txt", black);
        pieces.tex[i] = NULL;
        // Only load artwork for the actual 32 pieces
        if(i<=NUM_PIECES_TO_RENDER) piece_load_art(ren, &(pieces.tex[i]), file, color);
        pieces.col[i] = col; pieces.row[i] = row; pieces.color[i] = (color.r == white.r)?WHITE:BLACK;
        pieces.captured[i] = captured;
    }
    // The piece we are working on the art for (drawn at A3 A4 in white and A5 A6 in black):
    SDL_Texture *Bpiece_tex  = NULL; piece_load_art(ren, &Bpiece_tex, "piece.txt", black);  // Load temp piece art
    SDL_Texture *Wpiece_tex  = NULL; piece_load_art(ren, &Wpiece_tex, "piece.txt", white);  // Load temp piece art


    // Game state
    bool quit = false;
    SDL_Rect mouse_tile;
    bool mouse_down = false;
    bool mouse_just_pressed = false;
    bool dropped_piece = false;
    // State of the active piece
    enum piece_name ActivePiece_name = NONE;                    // Active piece
    enum piece_name LastActivePiece = NONE;                     // Last active AFTER a capture
    bool ActivePiece_drag = false;                              // Active piece dragged by mouse?
    // History
    Pieces history[1024];
    int history_idx = 0;
    int history_end = history_idx;
    history[history_idx] = pieces;

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
                        case SDLK_u:
                            history_idx--;
                            if(history_idx < 0) history_idx = 0;
                            pieces = history[history_idx];
                            break;
                        case SDLK_q: // Promotion
                            if( LastActivePiece < 16 )              // Means it's a pawn
                            {
                                if( pieces.color[LastActivePiece] == WHITE )
                                {
                                    if( pieces.row[LastActivePiece] == 0 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "queen.txt", white);
                                        pieces.name[LastActivePiece] = "WHITE_PQUEEN";
                                    }
                                }
                                else
                                {
                                    if( pieces.row[LastActivePiece] == 7 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "queen.txt", black);
                                        pieces.name[LastActivePiece] = "BLACK_PQUEEN";
                                    }
                                }
                            }
                            break;
                        case SDLK_r: // Promotion or Redo history
                            bool promotion = false;
                            if( LastActivePiece < 16 )              // Means it's a pawn
                            {
                                if( pieces.color[LastActivePiece] == WHITE )
                                {
                                    if( pieces.row[LastActivePiece] == 0 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "rook.txt", white);
                                        pieces.name[LastActivePiece] = "WHITE_PROOK";
                                        promotion = true;
                                    }
                                }
                                else
                                {
                                    if( pieces.row[LastActivePiece] == 7 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "rook.txt", black);
                                        pieces.name[LastActivePiece] = "BLACK_PROOK";
                                        promotion = true;
                                    }
                                }
                            }
                            if(  !promotion  )                  // Redo history
                            {
                                history_idx++; if(history_idx > history_end) history_idx = history_end;
                                pieces = history[history_idx];
                            }
                            break;
                        case SDLK_b: // Promotion
                            if( LastActivePiece < 16 )              // Means it's a pawn
                            {
                                if( pieces.color[LastActivePiece] == WHITE )
                                {
                                    if( pieces.row[LastActivePiece] == 0 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "bishop.txt", white);
                                        pieces.name[LastActivePiece] = "WHITE_PBISHOP";
                                    }
                                }
                                else
                                {
                                    if( pieces.row[LastActivePiece] == 7 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "bishop.txt", black);
                                        pieces.name[LastActivePiece] = "BLACK_PBISHOP";
                                    }
                                }
                            }
                            break;
                        case SDLK_k: // Promotion
                            if( LastActivePiece < 16 )              // Means it's a pawn
                            {
                                if( pieces.color[LastActivePiece] == WHITE )
                                {
                                    if( pieces.row[LastActivePiece] == 0 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "knight.txt", white);
                                        pieces.name[LastActivePiece] = "WHITE_PKNIGHT";
                                    }
                                }
                                else
                                {
                                    if( pieces.row[LastActivePiece] == 7 )
                                    {
                                        piece_load_art(ren, &pieces.tex[LastActivePiece], "knight.txt", black);
                                        pieces.name[LastActivePiece] = "BLACK_PKNIGHT";
                                    }
                                }
                            }
                            break;
                        default: break;
                    }
                }
                if(e.type == SDL_MOUSEBUTTONDOWN)
                {
                    mouse_down = true;
                    mouse_just_pressed = true;
                }
                if(e.type == SDL_MOUSEBUTTONUP)
                {
                    mouse_down = false;
                    if(ActivePiece_drag) dropped_piece = true;
                }
            }
        }
        { // Filtered
            SDL_PumpEvents();                                   // Update event queue
            const Uint8 *k = SDL_GetKeyboardState(NULL);        // Scan keys
            if(  k[SDL_SCANCODE_ESCAPE]  ) quit = true;              // q to quit
        }
        // Mouse: mouse_down
        { // Update state of ActivePiece
            int tile_dim = calc_tile_dim(wI.w, wI.h);
            int x,y;                                            // Mouse pixel x,y
            int col, row;                                       // Mouse chess col,row
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
            { // Convert mouse xy to chessboard col,row coordinates
                SDL_Rect border = calc_border(wI.w, wI.h);
                col = (int)(mouse_tile.x - border.x)/tile_dim;
                row = (int)(mouse_tile.y - border.y)/tile_dim;
            }
            if(mouse_down)
            {
                if(mouse_just_pressed)
                { // If mousedown on a piece, pick it up
                    mouse_just_pressed = false;
                    // If a piece is at the mousedown col,row start dragging it
                    for(int i=0; i<NUM_PIECES_TO_RENDER; i++)
                    {
                        if((  pieces.col[i] == col  ) && (  pieces.row[i] == row  ))
                        {
                            if(  pieces.captured[i] == false  )     // Ignore captured pieces
                            {
                                // Change the active piece
                                ActivePiece_drag = true;
                                ActivePiece_name = i;
                                break;
                            }
                        }
                    }
                }
                if(ActivePiece_drag == true)
                { // If dragging a piece, follow the mouse;
                    pieces.col[ActivePiece_name] = col;
                    pieces.row[ActivePiece_name] = row;
                }
            }
            if(dropped_piece)
            {
                ActivePiece_drag = false;
                dropped_piece = false;
                LastActivePiece = ActivePiece_name;
                ActivePiece_name = NONE;
                // If a piece is at the dropped location, take it
                for(unsigned int i=0; i<NUM_PIECES_TO_RENDER; i++)
                {
                    if(  i != ActivePiece_name  )               // Ignore active piece
                    {
                        if((  pieces.col[i] == col  ) && (  pieces.row[i] == row  ))
                        {
                            if(  pieces.color[i] != pieces.color[LastActivePiece]  )
                            {
                                pieces.captured[i] = true;
                            }
                        }
                    }
                }
                // Capture this moment in history
                history_idx++; if(history_idx > 1023) history_idx = 1023;
                history_end = history_idx;
                history[history_idx] = pieces;
            }
        }
        { // Update debug overlay text
            char *d = debug_txt;
            { const char *str = "History index, end: ";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { char str[8]; sprintf(str, "%d ", history_idx);     // int to string
                char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = ", ";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { char str[8]; sprintf(str, "%d", history_end);     // int to string
                char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "\t";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "Dragging piece: ";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = (ActivePiece_drag == true)?"true":"false";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "\t";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "Active piece: ";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = pieces.name[ActivePiece_name];
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "\n";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "Captured: ";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { for(int i=0; i<NUM_PIECES; i++)
                { if (pieces.captured[i])
                    {
                        { const char *str = pieces.name[i];
                            const char *c = str;
                            while( *c != '\0' ) *d++ = *c++;                // Copy characters
                            *d = *c;                                        // Copy nul-terminator
                        }
                        { const char *str = ", ";
                            const char *c = str;
                            while( *c != '\0' ) *d++ = *c++;                // Copy characters
                            *d = *c;                                        // Copy nul-terminator
                        }
                    }
                }
            }
            { const char *str = "\n";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "Last active piece (after a capture): ";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = pieces.name[LastActivePiece];
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
            }
            { const char *str = "\n";
                const char *c = str;
                while( *c != '\0' ) *d++ = *c++;                // Copy characters
                *d = *c;                                        // Copy nul-terminator
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
        { // Highlight tile of last move
            if(LastActivePiece != NONE)
            {
                int col = pieces.col[LastActivePiece];
                int row = pieces.row[LastActivePiece];
                int x = border.x + tile_dim*col;
                int y = border.y + tile_dim*row;
                SDL_SetRenderDrawColor(ren, 0xAF, 0xCF, 0x00, 0x7F);
                SDL_Rect hi_tile = {.x=x, .y=y, .w=tile_dim, .h=tile_dim};
                SDL_RenderFillRect(ren, &hi_tile);
            }
        }
        { // Temporary mouse test artwork for click happens good
            if(mouse_down)
            {
                SDL_SetRenderDrawColor(ren, 0, 0xFF, 0, 0x3F);
                SDL_RenderFillRect(ren, &mouse_tile);
            }
        }
        { // pieces
            // Make rectangle for where to draw piece
            /* int piece_dim = (wI.w > wI.h) ? wI.h : wI.w; piece_dim /= 12; */
            int piece_dim = calc_piece_dim(wI.w, wI.h);
            // Only render artwork for the actual 32 pieces
            for(int i=0; i<NUM_PIECES_TO_RENDER; i++)
            {
                // TODO: render captured pieces special
                if(  !(pieces.captured[i])  )
                {
                    piece_render(ren, pieces.tex[i], wI.w, wI.h, pieces.col[i], pieces.row[i]);
                }
            }
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
        { // Debug overlay
            SDL_Texture *debug_overlay; textbox txb;
            { // Layout text
                txb.text=debug_txt;
                txb.rect=(SDL_Rect){0}; txb.fg=white;
                const int margin = 10;
                txb.rect.x = margin;
                SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(font, txb.text, txb.fg, wI.w-margin);
                debug_overlay = SDL_CreateTextureFromSurface(ren,surf);
                SDL_FreeSurface(surf);
                SDL_QueryTexture(debug_overlay, NULL, NULL, &txb.rect.w, &txb.rect.h);
            }
            { // Render text
                // box
                SDL_SetRenderDrawColor(ren, 0,0,0,0x40); // Black 50%
                SDL_Rect debug_overlay_bgnd = {.x=0, .y=0, .w=wI.w, .h=txb.rect.h};
                /* SDL_Rect debug_overlay_bgnd = {.x=0, .y=0, .w=wI.w, .h=wI.h/2}; */
                SDL_RenderFillRect(ren, &debug_overlay_bgnd);
                // text
                SDL_RenderCopy(ren, debug_overlay, NULL, &txb.rect);
                // cleanup
                SDL_DestroyTexture(debug_overlay);
            }
        }
        { // Present to screen
            SDL_RenderPresent(ren);
            SDL_Delay(10);
        }
    }

    // Shutdown
    // Only destroy textures for the actual 32 pieces
    for(int i=0; i<NUM_PIECES_TO_RENDER; i++)
    {
        SDL_DestroyTexture(pieces.tex[i]);
    }
    SDL_DestroyTexture(Bpiece_tex);
    SDL_DestroyTexture(Wpiece_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
