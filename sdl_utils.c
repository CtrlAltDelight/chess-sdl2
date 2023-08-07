#include "sdl_utils.h"

bool init_libs() {
	// Initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Initialize SDL_image
	int img_flags = IMG_INIT_PNG;
	if(!(IMG_Init(img_flags) & img_flags)) {
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return true;
}


bool load_textures(SDL_Renderer* renderer, Textures* a_textures) {
	a_textures -> board = IMG_LoadTexture(renderer, "images/board.png");
	check_img_null(a_textures -> board); // Will return false if NULL

	a_textures -> pawn   = IMG_LoadTexture(renderer, "images/pawn.png");
	check_img_null(a_textures -> pawn);
	a_textures -> knight = IMG_LoadTexture(renderer, "images/knight.png");
	check_img_null(a_textures -> knight);
	a_textures -> bishop = IMG_LoadTexture(renderer, "images/bishop.png");
	check_img_null(a_textures -> bishop);
	a_textures -> rook   = IMG_LoadTexture(renderer, "images/rook.png");
	check_img_null(a_textures -> rook);
	a_textures -> queen  = IMG_LoadTexture(renderer, "images/queen.png");
	check_img_null(a_textures -> queen);
	a_textures -> king   = IMG_LoadTexture(renderer, "images/king.png");
	check_img_null(a_textures -> king);

	a_textures -> b_pawn   = IMG_LoadTexture(renderer, "images/black pawn.png");
	check_img_null(a_textures -> b_pawn);
	a_textures -> b_knight = IMG_LoadTexture(renderer, "images/black knight.png");
	check_img_null(a_textures -> b_knight);
	a_textures -> b_bishop = IMG_LoadTexture(renderer, "images/black bishop.png");
	check_img_null(a_textures -> b_bishop);
	a_textures -> b_rook   = IMG_LoadTexture(renderer, "images/black rook.png");
	check_img_null(a_textures -> b_rook);
	a_textures -> b_queen  = IMG_LoadTexture(renderer, "images/black queen.png");
	check_img_null(a_textures -> b_queen);
	a_textures -> b_king   = IMG_LoadTexture(renderer, "images/black king.png");
	check_img_null(a_textures -> b_king);

	return true; // Successful
}

void unload_textures(Textures textures) {
	SDL_DestroyTexture(textures.board);

	SDL_DestroyTexture(textures.pawn);
	SDL_DestroyTexture(textures.knight);
	SDL_DestroyTexture(textures.bishop);
	SDL_DestroyTexture(textures.rook);
	SDL_DestroyTexture(textures.queen);
	SDL_DestroyTexture(textures.king);

	SDL_DestroyTexture(textures.b_pawn);
	SDL_DestroyTexture(textures.b_knight);
	SDL_DestroyTexture(textures.b_bishop);
	SDL_DestroyTexture(textures.b_rook);
	SDL_DestroyTexture(textures.b_queen);
	SDL_DestroyTexture(textures.b_king);
}

