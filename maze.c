#include "maze.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum dir {
	DIR_ZERO,
	DIR_UP,
	DIR_LEFT,
	DIR_DOWN,
	DIR_RIGHT,
	DIR_N,
};

struct maze {
	bool *grid;
	size_t width;
	size_t height;
};

static enum dir randdir(const struct maze *, size_t, size_t);
static bool inbounds(const struct maze *, size_t, size_t);
static double randf(void);

static const size_t stepsz = 3;
static const int dirdr[] = {0, -1, 0, 1, 0};
static const int dirdc[] = {0, 0, -1, 0, 1};
static const enum dir revdir[]
    = {DIR_ZERO, DIR_DOWN, DIR_RIGHT, DIR_UP, DIR_LEFT};

void
maze_fill(bool *grid, size_t width, size_t height)
{
	memset(grid, false, width * height);
	struct maze m = {
	    .grid = grid,
	    .width = width,
	    .height = height,
	};
	enum dir *backtrack = calloc(width * height, sizeof(enum dir));

	size_t r = 1, c = 1;
	grid[r * width + c] = true;
	for (;;) {
		enum dir d = randdir(&m, r, c);
		if (d == DIR_ZERO) {
			enum dir back = backtrack[r * width + c];
			if (back == DIR_ZERO) {
				break;
			}
			r += dirdr[back] * stepsz;
			c += dirdc[back] * stepsz;
			continue;
		}
		for (size_t i = 0; i < stepsz; i++) {
			r += dirdr[d];
			c += dirdc[d];
			grid[r * width + c] = true;
			backtrack[r * width + c] = revdir[d];
		}
	}
	free(backtrack);
}

static enum dir
randdir(const struct maze *m, size_t r, size_t c)
{
	enum dir choice;
	int nvalid = 1;
	for (enum dir d = DIR_ZERO; d < DIR_N; d++) {
		size_t newr = dirdr[d] * stepsz + r;
		size_t newc = dirdc[d] * stepsz + c;
		if (!inbounds(m, newr, newc)) {
			continue;
		}
		if (m->grid[newr * m->width + newc]) {
			continue;
		}
		if (randf() <= (1.0 / nvalid)) {
			choice = d;
		}
		nvalid++;
	}
	return choice;
}

static bool
inbounds(const struct maze *m, size_t r, size_t c)
{
	return r < m->height && c < m->width;
}

static double
randf(void)
{
	return (double)rand() / RAND_MAX;
}
