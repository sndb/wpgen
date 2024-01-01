#include <stdbool.h>
#include <stdlib.h>

struct maze {
	bool *grid;
	size_t width;
	size_t height;
	size_t step;
	size_t ox;
	size_t oy;
};

void maze_fill(struct maze *m);
