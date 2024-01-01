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

static enum dir randdir(const struct maze *, size_t, size_t);
static bool inbounds(const struct maze *, size_t, size_t);
static double randf(void);

static const int dirdr[] = {0, -1, 0, 1, 0};
static const int dirdc[] = {0, 0, -1, 0, 1};
static const enum dir revdir[]
    = {DIR_ZERO, DIR_DOWN, DIR_RIGHT, DIR_UP, DIR_LEFT};

void
maze_fill(struct maze *m)
{
	size_t gridsz = m->width * m->height;
	enum dir *backtrack = calloc(gridsz, sizeof(enum dir));
	size_t r = m->oy, c = m->ox;
	m->grid[r * m->width + c] = true;
	for (;;) {
		enum dir d = randdir(m, r, c);
		if (d == DIR_ZERO) {
			enum dir back = backtrack[r * m->width + c];
			if (back == DIR_ZERO) {
				break;
			}
			r += dirdr[back] * m->step;
			c += dirdc[back] * m->step;
			continue;
		}
		for (size_t i = 0; i < m->step; i++) {
			r += dirdr[d];
			c += dirdc[d];
			m->grid[r * m->width + c] = true;
			backtrack[r * m->width + c] = revdir[d];
		}
	}
	free(backtrack);
}

static enum dir
randdir(const struct maze *m, size_t r, size_t c)
{
	enum dir choice = DIR_ZERO;
	int nvalid = 0;
	for (enum dir d = DIR_ZERO; d < DIR_N; d++) {
		size_t newr = dirdr[d] * m->step + r;
		size_t newc = dirdc[d] * m->step + c;
		if (!inbounds(m, newr, newc)) {
			continue;
		}
		if (m->grid[newr * m->width + newc]) {
			continue;
		}
		if (randf() <= (1.0 / ++nvalid)) {
			choice = d;
		}
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
