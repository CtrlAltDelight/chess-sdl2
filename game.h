#ifndef __GAME_H__
#define __GAME_H__

#include <stdbool.h>

typedef struct {
	enum {empty, pawn, knight, bishop, rook, queen, king} type;
	enum {black, white} color;
	bool has_moved; // For pawns, kings, and rooks
} Piece;

bool process_game_logic(SDL_Renderer* renderer, Textures textures, SDL_Event event, Piece** grid);

Piece** init_grid();

void destroy_grid(Piece** grid);

#endif

