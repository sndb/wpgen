#include "maze.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

enum {
	WIDTH = 960,
	HEIGHT = 540,
	FG = 0x0f3d30,
	BG = 0x092f1f,
};

void
wpgen(const char *filename, uint32_t fg, uint32_t bg,
    const bool *grid, size_t width, size_t height)
{
	size_t comp = 3;
	size_t sz = width * height;
	uint8_t *image = malloc(sizeof(uint8_t[comp * sz]));
	for (size_t i = 0; i < sz; i++) {
		uint32_t color = grid[i] ? fg : bg;
		for (size_t j = 0; j < comp; j++) {
			size_t rsh = 8 * (comp - j - 1);
			image[comp * i + j] = 0xff & (color >> rsh);
		}
	}
	stbi_write_png(filename, width, height, comp, image, width * comp);
	free(image);
}

int
main(void)
{
	bool grid[WIDTH * HEIGHT];
	srand(time(NULL));
	maze_fill(grid, WIDTH, HEIGHT);
	wpgen("out.png", FG, BG, grid, WIDTH, HEIGHT);
	return 0;
}
