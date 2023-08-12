#ifndef __GAME_H__
#define __GAME_H__

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include "sdl_utils.h"

enum Color {white, black};
enum Type {empty, pawn, knight, bishop, rook, queen, king};

typedef struct {
	enum Type type;
	enum Color color;
	bool has_moved; // Mostly for pawns, kings, and rooks
} Piece;

typedef struct {
	enum Type type;
	enum Color color;
	int row;
	int col;
	bool is_check;
} Move;

bool process_game_logic(SDL_Renderer* renderer, Textures textures, Piece** grid);
bool check_valid_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
Piece** init_grid();
void destroy_grid(Piece** grid);

#endif

