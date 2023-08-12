#ifndef __PIECES_H__
#define __PIECES_H__

#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#define simulate_board_state_and_return_is_valid(is_promoting, is_castling, is_en_passant) do {                                      \
	/* Make a temporary move */                                                                             \
	Piece temp = grid[end_row][end_col];                                                                    \
	if(is_promoting) {                                                                                      \
		grid[end_row][end_col].type = queen;                                                                \
	}                                                                                                       \
	else if(is_castling) {                                                                                  \
		int mod = (end_col - start_col > 0) ? 1 : -1;                                                       \
		grid[end_row][start_col + mod] = (Piece) { .type = rook, .color = piece.color, .has_moved = true }; \
		if(mod == 1) {                                                                                      \
			grid[end_row][7].type = empty;                                                                  \
		}                                                                                                   \
		else {                                                                                              \
			grid[end_row][0].type = empty;                                                                  \
		}                                                                                                   \
		grid[end_row][end_col] = grid[start_row][start_col];                                                \
	}                                                                                                       \
	else if(is_en_passant) {                                                                                \
		grid[end_row][end_col] = grid[start_row][start_col];                                                \
		grid[start_row][end_col].type = empty; /* delete enemy pawn if en passant */                        \
	}                                                                                                       \
	else {                                                                                                  \
		grid[end_row][end_col] = grid[start_row][start_col];                                                \
	}                                                                                                       \
	grid[start_row][start_col].type = empty;                                                                \
                                                                                                            \
	/* Check if valid */                                                                                    \
	bool is_valid = is_not_check(grid, piece.color);                                                        \
                                                                                                            \
	/* Reset grid to before temp move */                                                                    \
	if(!(is_valid) && is_promoting) {                                                                       \
		grid[start_row][start_col].type = pawn;                                                             \
	}                                                                                                       \
	else if((is_valid) && is_promoting) {                                                                   \
		grid[start_row][start_col].type = queen;                                                            \
	}                                                                                                       \
	else if(!(is_valid) && is_castling) {                                                                   \
		grid[end_row][start_col + end_col - start_col] = (Piece) { .type = empty };                         \
	}                                                                                                       \
	else if(!(is_valid) && is_en_passant) {                                                                 \
		grid[start_row][end_col].type = pawn; /* delete enemy pawn if en passant */                         \
		grid[start_row][start_col].type = pawn; /* delete enemy pawn if en passant */                       \
	}                                                                                                       \
	else {                                                                                                  \
		grid[start_row][start_col] = grid[end_row][end_col];                                                \
	}                                                                                                       \
	grid[end_row][end_col] = temp;                                                                          \
                                                                                                            \
	/* Either commit to move or cancel. */                                                                  \
	if(is_valid) {                                                                                          \
		return true;                                                                                        \
	}                                                                                                       \
	return false;                                                                                           \
} while(false)

bool check_pawn_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
bool check_knight_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
bool check_bishop_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
bool check_rook_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
bool check_queen_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
bool check_king_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
bool check_valid_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col);
void move_piece(Piece** grid, enum Color* a_turn, Move* a_previous_move, int orig_row, int orig_col, int row, int col);

#endif

