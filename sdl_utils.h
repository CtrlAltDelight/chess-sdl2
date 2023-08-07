#ifndef __SDL_UTILS_H__
#define __SDL_UTILS_H__

#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Constants
#define INT_SCALING 6

#define UNSCALED_BOARD_LENGTH (8 * 16 + 4 * 2)
#define BOARD_LENGTH ((UNSCALED_BOARD_LENGTH) * (INT_SCALING))

#define UNSCALED_GRID_LENGTH 16
#define GRID_LENGTH ((UNSCALED_GRID_LENGTH) * (INT_SCALING))

#define UNSCALED_BORDER_LENGTH 4
#define BORDER_LENGTH (UNSCALED_BORDER_LENGTH * INT_SCALING)

#define SCREEN_WIDTH BOARD_LENGTH
#define SCREEN_HEIGHT SCREEN_WIDTH

typedef struct {
	SDL_Texture* board;

	// White pieces
	SDL_Texture* pawn;
	SDL_Texture* knight;
	SDL_Texture* bishop;
	SDL_Texture* rook;
	SDL_Texture* queen;
	SDL_Texture* king;

	// Black pieces
	SDL_Texture* b_pawn;
	SDL_Texture* b_knight;
	SDL_Texture* b_bishop;
	SDL_Texture* b_rook;
	SDL_Texture* b_queen;
	SDL_Texture* b_king;
} Textures;

#define check_null(x) do {                                           \
	if((x) == NULL) {                                                \
		printf("%s is NULL! SDL_Error: %s\n", (#x), SDL_GetError()); \
		return EXIT_FAILURE;                                         \
	}                                                                \
} while(false)

#define check_img_null(x) do {                                       \
	if((x) == NULL) {                                                \
		printf("%s is NULL! IMG_Error: %s\n", (#x), IMG_GetError()); \
		return EXIT_FAILURE;                                         \
	}                                                                \
} while(false)

bool init_libs();
bool load_textures(SDL_Renderer* renderer, Textures* a_textures);
void unload_textures(Textures textures);

#endif

