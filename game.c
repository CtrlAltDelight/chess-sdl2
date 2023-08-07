#include <math.h>
#include "sdl_utils.h"
#include "game.h"

Piece** init_grid() {
	// Initialize the grid to all empty
	Piece** grid = malloc(sizeof(*grid) * 8);
	for(int i = 0; i < 8; i++) {
		grid[i] = malloc(sizeof(**grid) * 8);
		for(int j = 0; j < 8; j++) {
			grid[i][j] = (Piece) { .type = empty };
		}
	}

	// Initialize major black pieces
	grid[7][0] = (Piece) { .type = rook,   .color = black, .has_moved = false };
	grid[7][1] = (Piece) { .type = knight, .color = black, .has_moved = false };
	grid[7][2] = (Piece) { .type = bishop, .color = black, .has_moved = false };
	grid[7][3] = (Piece) { .type = queen,  .color = black, .has_moved = false };
	grid[7][4] = (Piece) { .type = king,   .color = black, .has_moved = false };
	grid[7][5] = (Piece) { .type = bishop, .color = black, .has_moved = false };
	grid[7][6] = (Piece) { .type = knight, .color = black, .has_moved = false };
	grid[7][7] = (Piece) { .type = rook,   .color = black, .has_moved = false };

	// initialize black pawns
	for(int i = 0; i < 8; i++) {
		grid[6][i] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	}

	// Initialize major white pieces
	grid[0][0] = (Piece) { .type = rook,   .color = white, .has_moved = false };
	grid[0][1] = (Piece) { .type = knight, .color = white, .has_moved = false };
	grid[0][2] = (Piece) { .type = bishop, .color = white, .has_moved = false };
	grid[0][3] = (Piece) { .type = queen,  .color = white, .has_moved = false };
	grid[0][4] = (Piece) { .type = king,   .color = white, .has_moved = false };
	grid[0][5] = (Piece) { .type = bishop, .color = white, .has_moved = false };
	grid[0][6] = (Piece) { .type = knight, .color = white, .has_moved = false };
	grid[0][7] = (Piece) { .type = rook,   .color = white, .has_moved = false };

	// initialize white pawns
	for(int i = 0; i < 8; i++) {
		grid[1][i] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	}

	return grid;
}

void destroy_grid(Piece** grid) {
	for(int i = 0; i < 8; i++) {
		free(grid[i]);
	}
	free(grid);
}

static SDL_Texture* get_texture_for_piece(Piece piece, Textures textures) {
	switch(piece.type) {
		case pawn:
			return (piece.color == white) ? textures.pawn : textures.b_pawn;
		case knight:
			return (piece.color == white) ? textures.knight : textures.b_knight;
		case bishop:
			return (piece.color == white) ? textures.bishop : textures.b_bishop;
		case rook:
			return (piece.color == white) ? textures.rook : textures.b_rook;
		case queen:
			return (piece.color == white) ? textures.queen : textures.b_queen;
		case king:
			return (piece.color == white) ? textures.king : textures.b_king;
		default:
			return NULL; // No texture for empty square
	}
}

static void render_grid(SDL_Renderer* renderer, Piece** grid, Textures textures) {
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++) {
			Piece piece = grid[i][j];
			SDL_Texture* texture = get_texture_for_piece(piece, textures);
			int x_pos = BOARD_LENGTH - BORDER_LENGTH - GRID_LENGTH * (j + 1); // add 1 to i since you need the top left corner of the current grid
			int y_pos = BOARD_LENGTH - BORDER_LENGTH - GRID_LENGTH * (i + 1);
			SDL_Rect destination = { .x = x_pos, .y = y_pos, .h = GRID_LENGTH, .w = GRID_LENGTH };
			SDL_RenderCopy(renderer, texture, NULL, &destination);
		}
	}
}

static void move_piece(Piece** grid, enum Color* a_turn, Move* a_previous_move, int orig_row, int orig_col, int row, int col) {
	// Do nothing if there is no move
	if(orig_row == row && orig_col == col) {
		return;
	}

	// Update piece position
	grid[row][col] = grid[orig_row][orig_col];
	grid[orig_row][orig_col] = (Piece) { .type = empty };
	grid[row][col].has_moved = true;

	// Update turn and previous move
	*a_previous_move = (Move) { .type = grid[row][col].type, .color = *a_turn, .row = row, .col = col, .is_check = false };
	*a_turn = (*a_turn == white) ? black : white;
}

static void draw_dragged_piece(SDL_Renderer* renderer, Textures textures, Piece dragged_piece, int x, int y) {
	SDL_Texture* texture = get_texture_for_piece(dragged_piece, textures);
	SDL_Rect destination = { 
		.x = x - (GRID_LENGTH / 4), // Render it centered on mouse
		.y = y - (GRID_LENGTH / 4), 
		.w = GRID_LENGTH / 2, // Make it half the size
		.h = GRID_LENGTH / 2
	};
	SDL_RenderCopy(renderer, texture, NULL, &destination);
}

static bool is_on_or_outside_border(int x, int y) {
	return y < BORDER_LENGTH || x < BORDER_LENGTH || y > BOARD_LENGTH - BORDER_LENGTH 
	                                                           || x > BOARD_LENGTH - BORDER_LENGTH;
}

static bool is_not_check(Piece** grid, enum Color turn) {
	// Find where king is at
	int king_row;
	int king_col;
	for(int i = 0; i < 8; i++) {
		bool found = false;
		for(int j = 0; j < 8; j++) {
			Piece curr_piece = grid[i][j];
			if(curr_piece.type == king && curr_piece.color == turn) {
				king_row = i;
				king_col = j;
				found = true;
				break;
			}
		}
		if(found) {
			break;
		}
	}

	// Evaluate if any pieces are attacking the king
	bool is_check = false;
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++) {
			Piece curr_piece = grid[i][j];
			is_check = check_valid_move(grid, curr_piece, (Move) {.type = rook}, i, j, king_row, king_col); // previous move doesn't matter
			if(is_check) {
				break;
			}
		}
		if(is_check) {
			break;
		}
	}
	return !is_check;
}

bool check_valid_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	// Invalid if attempting to capture own piece
	Piece dest_piece = grid[end_row][end_col];
	if(dest_piece.type != empty && dest_piece.color == piece.color) {
		return false;
	}
	
	if(piece.type == pawn) {
		// Color matters
		int color_modifier = (piece.color == white) ? 1 : -1;
		bool is_en_passant = previous_move.col == end_col && previous_move.type == pawn && abs(previous_move.col - start_col) == 1 && ((end_row == 2 && previous_move.row == 3 && piece.color == black) || (end_row == 5 && previous_move.row == 4 && piece.color == white));

		// One square forward
		if(end_row == start_row + color_modifier * 1 && end_col == start_col && grid[end_row][end_col].type == empty) {
			// Promotion
			if(end_row == 7 || end_row == 0) {
				grid[start_row][start_col].type = queen;
			}
			check_for_check();
		}
		// Two squares forward
		else if(piece.has_moved == false && end_row == start_row + color_modifier * 2 && end_col == start_col && grid[end_row][end_col].type == empty && grid[start_row + color_modifier][start_col].type == empty) {
			check_for_check();
		}
		// Capture
		else if(end_row == start_row + color_modifier * 1 && (end_col == start_col + 1 || end_col == start_col - 1) && (grid[end_row][end_col].type != empty || is_en_passant)) {
			// en passant
			if(is_en_passant) {
				grid[start_row][end_col].type = empty; // delete enemy pawn if en passant
			}
			// Promotion
			else if(end_row == 7 || end_row == 0) {
				grid[start_row][start_col].type = queen;
			}
			check_for_check();
		}
	}
	else if(piece.type == knight) {
		if(abs(end_row - start_row) + abs(end_col - start_col) == 3 && start_col != end_col && start_row != end_row) {
			check_for_check();
		}
	}
	else if(piece.type == bishop) {
		for(int i = 1; i < abs(end_row - start_row); i++) {
			int row_offset = (end_row > start_row) ? i : -i;
			int col_offset = (end_col > start_col) ? i : -i;
			if(start_row + row_offset <= 7 && start_col + col_offset <= 7 && grid[start_row + row_offset][start_col + col_offset].type != empty) {
				return false;
			}
		}
		if(abs(end_row - start_row) == abs(end_col - start_col)) {
			return is_not_check(grid, piece.color);
		}
	}
	else if(piece.type == rook) {
		if(start_row == end_row && start_col != end_col) { // horizontal movement
			int step = (end_col > start_col) ? 1 : -1;
			for(int curr_col = start_col + step; curr_col != end_col; curr_col += step) {
				if(grid[start_row][curr_col].type != empty) {
					return false;
				}
			}
			check_for_check();
		}
		else if(start_row != end_row && start_col == end_col) { // vertical movement
			int step = (end_row > start_row) ? 1 : -1;
			for(int curr_row = start_row + step; curr_row != end_row; curr_row += step) {
				if(grid[curr_row][start_col].type != empty) {
					return false;
				}
			}
			check_for_check();
		}
	}
	else if(piece.type == queen) {
		Piece as_bishop = { .type = bishop, .color = piece.color, .has_moved = piece.has_moved };
		Piece as_rook   = { .type = rook,   .color = piece.color, .has_moved = piece.has_moved };

		return check_valid_move(grid, as_bishop, previous_move, start_row, start_col, end_row, end_col) ||
			   check_valid_move(grid, as_rook,   previous_move, start_row, start_col, end_row, end_col);
	}
	else if(piece.type == king) {
		if(abs(end_row - start_row) <= 1 && abs(end_col - start_col) <= 1) {
			check_for_check();
		}
	}
	return false;
}

// returning true exits the game
bool process_game_logic(SDL_Renderer* renderer, Textures textures, Piece** grid) {
	static SDL_Event event;
	static bool dragging = false;
	static int original_row = -1; // Used when dragging a piece
	static int original_col = -1;
	static Piece* dragged_piece = NULL;
	static int mouse_x = 0;
	static int mouse_y = 0;
	static enum Color turn = white;
	static Move previous_move;

	// Clear and draw board
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, textures.board, NULL, NULL);

	// Figure out game state
	while (SDL_PollEvent(&event) != 0) {
		if(event.type == SDL_QUIT) {
			return true;
		}
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
			return true;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN) {
			// Get position of click
			int click_x = event.button.x;
			int click_y = event.button.y;

			// Get indices of the click
			int row = (BOARD_LENGTH - click_y - BORDER_LENGTH - 1) / GRID_LENGTH;
			int col = (BOARD_LENGTH - click_x - BORDER_LENGTH - 1) / GRID_LENGTH;

			// Check if out of bounds
			bool is_row_valid = 0 <= row && row <= 7;
			bool is_col_valid = 0 <= col && col <= 7;
			bool position_is_invalid = is_on_or_outside_border(click_x, click_y) || !is_col_valid || !is_row_valid;
			if(position_is_invalid) {
				continue;
			}

			// Check if there is a piece at the clicked square
			if(grid[row][col].type == empty) {
				continue;
			}

			// Check if the piece is of the right color
			if(grid[row][col].color != turn) {
				continue;
			}

			dragging = true;
			dragged_piece = &grid[row][col];
			original_row = row;
			original_col = col;
		}
		if(event.type == SDL_MOUSEMOTION) {
			mouse_x = event.motion.x;
			mouse_y = event.motion.y;
		}
		if(event.type == SDL_MOUSEBUTTONUP && dragging) {
			// Piece is no longer being dragged
			dragging = false;

			// Get position of click
			int click_x = event.button.x;
			int click_y = event.button.y;

			// Get indices of the click
			int row = (BOARD_LENGTH - click_y - BORDER_LENGTH) / GRID_LENGTH;
			int col = (BOARD_LENGTH - click_x - BORDER_LENGTH) / GRID_LENGTH;

			// Check if the end position is valid
			bool is_row_valid = 0 <= row && row <= 7;
			bool is_col_valid = 0 <= col && col <= 7;
			bool position_is_invalid = is_on_or_outside_border(click_x, click_y) || !is_col_valid || !is_row_valid;
			if(position_is_invalid) {
				dragged_piece = NULL;
				continue;
			}

			// Check if move is legal and update accordingly
			if(check_valid_move(grid, *dragged_piece, previous_move, original_row, original_col, row, col)) {
				move_piece(grid, &turn, &previous_move, original_row, original_col, row, col);
			}
			dragged_piece = NULL;
		}
	}

	// Draw dragged piece if needed
	if(dragging && dragged_piece != NULL) {
		draw_dragged_piece(renderer, textures, *dragged_piece, mouse_x, mouse_y);
	}

	// Draw all the piece on the board and present
	render_grid(renderer, grid, textures);
	SDL_RenderPresent(renderer);
	return false;
}

