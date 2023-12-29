#include "maze.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum {
	zero,
	up,
	left,
	down,
	right
} dir;

typedef struct {
	bool *grid;
	size_t width;
	size_t height;
} maze;

static dir randdir(const maze *, size_t, size_t);
static bool inbounds(const maze *, size_t, size_t);
static double randf(void);
static dir revdir(dir);

static const size_t stepsz = 2;
static const int dirdr[] = {0, -1, 0, 1, 0};
static const int dirdc[] = {0, 0, -1, 0, 1};

void
maze_fill(bool *grid, size_t width, size_t height)
{
	maze m = {
	    .grid = grid,
	    .width = width,
	    .height = height,
	};
	dir *backtrack = calloc(width * height, sizeof(dir));

	size_t r = 1, c = 1;
	grid[r * width + c] = true;
	for (;;) {
		dir d = randdir(&m, r, c);
		if (d == zero) {
			dir back = backtrack[r * width + c];
			if (back == zero)
				break;
			r += dirdr[back] * stepsz;
			c += dirdc[back] * stepsz;
			continue;
		}
		for (size_t i = 0; i < stepsz; i++) {
			r += dirdr[d];
			c += dirdc[d];
			grid[r * width + c] = true;
			backtrack[r * width + c] = revdir(d);
		}
	}
	free(backtrack);
}

static dir
randdir(const maze *m, size_t r, size_t c)
{
	const dir dirs[] = {up, left, down, right};
	dir choice = zero;
	int nvalid = 1;
	for (int i = 0; i < 4; i++) {
		dir d = dirs[i];
		size_t newr = dirdr[d] * stepsz + r;
		size_t newc = dirdc[d] * stepsz + c;
		if (!inbounds(m, newr, newc))
			continue;
		if (m->grid[newr * m->width + newc])
			continue;
		if (randf() < (1.0 / nvalid))
			choice = d;
		nvalid++;
	}
	return choice;
}

static bool
inbounds(const maze *m, size_t r, size_t c)
{
	return r < m->height && c < m->width;
}

static double
randf(void)
{
	return (double)rand() / RAND_MAX;
}

static dir
revdir(dir d)
{
	const dir rev[] = {zero, down, right, up, left};
	return rev[d];
}
