#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include "sdl_utils.h"
#include "game.h"

int main(int argc, char* argv[]) {
	// Initialize SDL
	if(init_libs() == false) {
		return EXIT_FAILURE;
	} 

	// Create window
	SDL_Window* window = SDL_CreateWindow(
				"Chess",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
				SCREEN_WIDTH, SCREEN_HEIGHT, 
				SDL_WINDOW_SHOWN
	);
	check_null(window);

	// Create renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	check_null(renderer);

	// Set renderer color
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

	// Load textures
	Textures textures;
	if(load_textures(renderer, &textures) == false) {
		return EXIT_FAILURE;
	}

	// Run game
	Piece** grid = init_grid();
	for(bool is_done = false; !is_done; is_done = process_game_logic(renderer, textures, grid));
	destroy_grid(grid);

	// SDL cleanup
	unload_textures(textures);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
