#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include "sdl_utils.h"

bool process_game_logic() {
	return true;
}

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
	SDL_WarpMouseInWindow(window, 0, 0); // Not completely necessary

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
	bool is_done = false;
	while(!is_done) {
		is_done = process_game_logic();
	}

	// Render the texture
	int y_position = BOARD_LENGTH - BORDER_LENGTH - 2 * GRID_LENGTH;
	int x_position = BORDER_LENGTH + 1 * GRID_LENGTH;
	SDL_Rect destination = { .x = x_position, .y = y_position, .w = GRID_LENGTH, .h = GRID_LENGTH };
	SDL_RenderCopy(renderer, textures.pawn, NULL, &destination);

	SDL_RenderPresent(renderer);


	SDL_Delay(2000); // STUB TODO - Remove later

	// SDL cleanup
	unload_textures(textures);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
