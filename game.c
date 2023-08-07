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
	grid[6][0] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	grid[6][1] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	grid[6][2] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	grid[6][3] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	grid[6][4] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	grid[6][5] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	grid[6][6] = (Piece) { .type = pawn, .color = black, .has_moved = false };
	grid[6][7] = (Piece) { .type = pawn, .color = black, .has_moved = false };

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
	grid[1][0] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	grid[1][1] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	grid[1][2] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	grid[1][3] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	grid[1][4] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	grid[1][5] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	grid[1][6] = (Piece) { .type = pawn, .color = white, .has_moved = false };
	grid[1][7] = (Piece) { .type = pawn, .color = white, .has_moved = false };

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

static void move_piece(Piece** grid, int orig_row, int orig_col, int row, int col) {
	// Do nothing if there is no move
	if(orig_row == row && orig_col == col) {
		return;
	}

	grid[row][col] = grid[orig_row][orig_col];
	grid[orig_row][orig_col] = (Piece) { .type = empty };
}

static void draw_dragged_piece(SDL_Renderer* renderer, Textures textures, Piece dragged_piece, int x, int y) {
	SDL_Texture* texture = get_texture_for_piece(dragged_piece, textures);
	SDL_Rect destination = { .x = x - (GRID_LENGTH / 4), .y = y - (GRID_LENGTH / 4), .w = GRID_LENGTH / 2, .h = GRID_LENGTH / 2};
	SDL_RenderCopy(renderer, texture, NULL, &destination);
}

// returning true exits the game
bool process_game_logic(SDL_Renderer* renderer, Textures textures, SDL_Event event, Piece** grid) {
	static bool dragging = false;
	static int original_row = -1;
	static int original_col = -1;
	static Piece* dragged_piece = NULL;
	static int mouse_x = 0;
	static int mouse_y = 0;

	// Clear and draw board
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, textures.board, NULL, NULL);

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
			printf("Click Down\nrow: %d, col: %d\n\n", row, col);

			// Check if out of bounds
			bool on_border = click_y < BORDER_LENGTH || click_x < BORDER_LENGTH;
			bool too_high = row > 7 || col > 7;
			bool too_low = row < 0 || col < 0;
			if(too_low || too_high || on_border) {
				continue;
			}

			// Check if there is a piece at the clicked square
			if(grid[row][col].type == empty) {
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

			// Update the drawn position of the piece (not actual board state)
		}
		if(event.type == SDL_MOUSEBUTTONUP && dragging) {
			// Get position of click
			int click_x = event.button.x;
			int click_y = event.button.y;

			// Get indices of the click
			int row = (BOARD_LENGTH - click_y - BORDER_LENGTH) / GRID_LENGTH;
			int col = (BOARD_LENGTH - click_x - BORDER_LENGTH) / GRID_LENGTH;
			bool on_border = click_y < BORDER_LENGTH || click_x < BORDER_LENGTH;
			bool too_high = row > 7 || col > 7;
			bool too_low = row < 0 || col < 0;
			if(too_low || too_high || on_border) {
				dragging = false;
				dragged_piece = NULL;
				continue;
			}

			// TODO check if move is legal
			// Update board
			move_piece(grid, original_row, original_col, row, col);
			dragging = false;
			dragged_piece = NULL;
		}
	}

	if(dragging && dragged_piece != NULL) {
		draw_dragged_piece(renderer, textures, *dragged_piece, mouse_x, mouse_y);
	}
	render_grid(renderer, grid, textures);
	SDL_RenderPresent(renderer);
	return false;
}

