#include "maze.h"

#include <stdio.h>

#define WIDTH 41
#define HEIGHT 25

int
main(void)
{
	bool grid[WIDTH * HEIGHT] = {0};
	maze_fill(grid, WIDTH, HEIGHT);
	for (size_t r = 0; r < HEIGHT; r++) {
		for (size_t c = 0; c < WIDTH; c++)
			fputs(grid[r * WIDTH + c] ? " " : "█", stdout);
		putchar('\n');
	}
	return 0;
}
