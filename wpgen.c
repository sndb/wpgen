#include "maze.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <fcntl.h>
#include <stb/stb_image_write.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

enum {
	MAX_WIDTH = 32768,
	MAX_HEIGHT = 32768,
};

void
writefn(void *context, void *data, int size)
{
	int *fd = context;
	write(*fd, data, size);
}

void
run(int fd, uint32_t fg, uint32_t bg, size_t width, size_t height, size_t step)
{
	size_t comp = 3;
	size_t stride = width * comp;
	size_t len = width * height;
	bool *grid = malloc(len);
	uint8_t *image = malloc(sizeof(uint8_t[comp * len]));
	maze_fill(grid, width, height, step);
	for (size_t i = 0; i < len; i++) {
		uint32_t color = grid[i] ? fg : bg;
		for (size_t j = 0; j < comp; j++) {
			size_t rsh = 8 * (comp - j - 1);
			image[comp * i + j] = 0xff & (color >> rsh);
		}
	}

	stbi_write_png_to_func(writefn, &fd, width, height, comp, image, stride);
	free(image);
	free(grid);
}

bool
isrgb(uint32_t x)
{
	return (x >> 24) == 0;
}

void
die(const char *s)
{
	fputs(s, stderr);
	exit(1);
}

int
main(int argc, char *argv[])
{
	srand(time(NULL));

	if (argc != 5 && argc != 6) {
		die("usage: wpgen <fg> <bg> <width> <height> [step]\n");
	}

	uint32_t fg = strtol(argv[1], NULL, 16);
	if (!isrgb(fg)) {
		die("wpgen: bad fg\n");
	}

	uint32_t bg = strtol(argv[2], NULL, 16);
	if (!isrgb(bg)) {
		die("wpgen: bad bg\n");
	}

	size_t width = atoi(argv[3]);
	if (width < 1 || width > MAX_WIDTH) {
		die("wpgen: bad width\n");
	}

	size_t height = atoi(argv[4]);
	if (height < 1 || height > MAX_HEIGHT) {
		die("wpgen: bad height\n");
	}

	size_t step = 3;
	if (argc > 5) {
		step = atoi(argv[5]);
		if (step < 2 || step > width - 2) {
			die("wpgen: step size must be in [2, WIDTH - 2]\n");
		}
	}

	int fd = 1;
	if (isatty(fd)) {
		fd = open("wpgen.png", O_RDWR | O_CREAT | O_TRUNC, 0666);
	}
	run(fd, fg, bg, width, height, step);
	return 0;
}
