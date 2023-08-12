#ifndef __GAME_H__
#define __GAME_H__

#include <stdbool.h>

#define check_for_check(is_promoting, is_castling, is_en_passant) do {                                                     \
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
	else if(is_en_passant) {\
				grid[end_row][end_col] = grid[start_row][start_col];\
				grid[start_row][end_col].type = empty; /* delete enemy pawn if en passant */\
	}\
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
	else if(!(is_valid) && is_en_passant) {\
				grid[start_row][end_col].type = pawn; /* delete enemy pawn if en passant */\
				grid[start_row][start_col].type = pawn; /* delete enemy pawn if en passant */\
	}\
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

enum Color {white, black};
enum Type {empty, pawn, knight, bishop, rook, queen, king};

typedef struct {
	enum Type type;
	enum Color color;
	bool has_moved; // For pawns, kings, and rooks
	bool en_passant_available;
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

