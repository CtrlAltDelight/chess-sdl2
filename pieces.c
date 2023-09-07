#include "game.h"
#include "pieces.h"

// This function is useful for simulating
static bool is_enemy_attacking_square(Piece** grid, enum Color turn, int row, int col) {
	// Evaluate if any pieces are attacking the square
	bool is_attacking = false;
	for(int i = 0; i < 8; i++) { // Go through each square in the grid
		for(int j = 0; j < 8; j++) {
			Piece curr_piece = grid[i][j];
			if(curr_piece.color == turn) { // We only care about enemy pieces
				continue;
			}
			is_attacking = check_valid_move(grid, curr_piece, (Move) {.type = rook}, i, j, row, col); // previous move doesn't matter
			
			// As soon as you find one piece that is attacking the square, return.
			if(is_attacking) {
				return is_attacking;
			}
		}
	}
	return is_attacking;
}

static bool is_not_check(Piece** grid, enum Color turn) {
	// Find where king is at
	int king_row;
	int king_col;
	for(int i = 0; i < 8; i++) {
		bool found = false; // flags if we have found the king
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
	return !is_enemy_attacking_square(grid, turn, king_row, king_col);
}

bool check_pawn_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	// Color matters
	int color_modifier = (piece.color == white) ? 1 : -1;

	// Check if this is an en passant attempt
	bool is_en_passant = previous_move.col == end_col && previous_move.type == pawn && abs(previous_move.col - start_col) == 1 && ((end_row == 2 && previous_move.row == 3 && piece.color == black) || (end_row == 5 && previous_move.row == 4 && piece.color == white));

	// TODO See if there is a better way to check for promotion
	
	// One square forward
	if(end_row == start_row + color_modifier * 1 && end_col == start_col && grid[end_row][end_col].type == empty) {
		// Promotion
		if(end_row == 7 || end_row == 0) {
			grid[start_row][start_col].type = queen;
			simulate_board_state_and_return_is_valid(true, false, false);
		}

		simulate_board_state_and_return_is_valid(false, false, false);
	}
	
	// Two squares forward
	else if(piece.has_moved == false && end_row == start_row + color_modifier * 2 && end_col == start_col && grid[end_row][end_col].type == empty && grid[start_row + color_modifier][start_col].type == empty) {
		simulate_board_state_and_return_is_valid(false, false, false);
	}

	// Capture
	else if(end_row == start_row + color_modifier * 1 && (end_col == start_col + 1 || end_col == start_col - 1) && (grid[end_row][end_col].type != empty || is_en_passant)) {
		// en passant
		if(is_en_passant) {
			simulate_board_state_and_return_is_valid(false, false, true);
		}

		// Promotion
		else if(end_row == 7 || end_row == 0) {
			//grid[start_row][start_col].type = queen;
			simulate_board_state_and_return_is_valid(true, false, false);
		}

		simulate_board_state_and_return_is_valid(false, false, false);
	}
	return false;
}


bool check_knight_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	if(abs(end_row - start_row) + abs(end_col - start_col) == 3 && start_col != end_col && start_row != end_row) {
		simulate_board_state_and_return_is_valid(false, false, false);
	}
	return false;
}

bool check_bishop_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	for(int i = 1; i < abs(end_row - start_row); i++) {
		int row_offset = (end_row > start_row) ? i : -i;
		int col_offset = (end_col > start_col) ? i : -i;
		if(start_row + row_offset <= 7 && start_col + col_offset <= 7  &&
		   start_row + row_offset >= 0 && start_col + col_offset >= 0 && grid[start_row + row_offset][start_col + col_offset].type != empty) {
			return false;
		}
	}
	if(abs(end_row - start_row) == abs(end_col - start_col)) {
		simulate_board_state_and_return_is_valid(false, false, false);
	}
	return false;
}

bool check_rook_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	if(start_row == end_row && start_col != end_col) { // horizontal movement
		int step = (end_col > start_col) ? 1 : -1;
		for(int curr_col = start_col + step; curr_col != end_col; curr_col += step) {
			if(grid[start_row][curr_col].type != empty) {
				return false;
			}
		}
		simulate_board_state_and_return_is_valid(false, false, false);
	}
	else if(start_row != end_row && start_col == end_col) { // vertical movement
		int step = (end_row > start_row) ? 1 : -1;
		for(int curr_row = start_row + step; curr_row != end_row; curr_row += step) {
			if(grid[curr_row][start_col].type != empty) {
				return false;
			}
		}
		simulate_board_state_and_return_is_valid(false, false, false);
	}
	return false;
}

bool check_queen_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	Piece as_bishop = { .type = bishop, .color = piece.color, .has_moved = piece.has_moved };
	Piece as_rook   = { .type = rook,   .color = piece.color, .has_moved = piece.has_moved };

	return check_valid_move(grid, as_bishop, previous_move, start_row, start_col, end_row, end_col) ||
		   check_valid_move(grid, as_rook,   previous_move, start_row, start_col, end_row, end_col);
}

bool check_king_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	if(abs(end_row - start_row) <= 1 && abs(end_col - start_col) <= 1) {
		simulate_board_state_and_return_is_valid(false, false, false);
	}
	// castling
	else if(abs(end_col - start_col) == 2 && piece.has_moved == false) {
		// Get direction of the travel
		int step = (end_col > start_col) ? 1 : -1;
		
		// Make sure there is only empty space between the king and the rook
		for(int curr_col = start_col + step; 0 < curr_col && curr_col < 7; curr_col += step) {
			if(grid[start_row][curr_col].type != empty) {
				return false;
			}
		}

		// Make sure you are not in check or traveling through check
		if(!is_not_check(grid, piece.color)                                               ||
				is_enemy_attacking_square(grid, piece.color, start_row, start_col + step) ||
				is_enemy_attacking_square(grid, piece.color, start_row, start_col + 2 * step)) {
			return false;
		}

		// Make sure the rooks are in the right spot and have not moved yet
		bool is_left_rook_available  = grid[start_row][0].type == rook && grid[start_row][0].has_moved == false;
		bool is_right_rook_available = grid[start_row][7].type == rook && grid[start_row][7].has_moved == false;
		if((step == 1 && is_right_rook_available) || (step == -1 && is_left_rook_available)) {
			simulate_board_state_and_return_is_valid(false, true, false);
		}
	}
	return false;
}

// This function should NOT change anything about the board
bool check_valid_move(Piece** grid, Piece piece, Move previous_move, int start_row, int start_col, int end_row, int end_col) {
	// Invalid if attempting to capture own piece
	Piece dest_piece = grid[end_row][end_col];
	if(dest_piece.type != empty && dest_piece.color == piece.color) {
		return false;
	}
	
	// See if that piece can make that move
	switch(piece.type) {
		case pawn:   return check_pawn_move(grid,   piece, previous_move, start_row, start_col, end_row, end_col);
		case knight: return check_knight_move(grid, piece, previous_move, start_row, start_col, end_row, end_col);
		case bishop: return check_bishop_move(grid, piece, previous_move, start_row, start_col, end_row, end_col);
		case rook:   return check_rook_move(grid,   piece, previous_move, start_row, start_col, end_row, end_col);
		case queen:  return check_queen_move(grid,  piece, previous_move, start_row, start_col, end_row, end_col);
		case king:   return check_king_move(grid,   piece, previous_move, start_row, start_col, end_row, end_col);
		default:     return false;
	}
}

// This function is the only function that changes the board
void move_piece(Piece** grid, enum Color* a_turn, Move* a_previous_move, int orig_row, int orig_col, int row, int col) {
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

