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

const char *usage = "Usage: wpgen [OPTION]... WIDTH HEIGHT\n"
                    "  -fg COLOR       Foreground color\n"
                    "  -bg COLOR       Background color\n"
                    "  -step PX        Distance between corridors\n"
                    "  -x-offset PX    Initial X position\n"
                    "  -y-offset PX    Initial Y position\n";

void
writefn(void *context, void *data, int size)
{
	int *fd = context;
	write(*fd, data, size);
}

void
run(int fd, uint32_t fg, uint32_t bg,
    const bool *grid, size_t width, size_t height)
{
	size_t comp = 3;
	size_t gridsz = width * height;
	uint8_t *image = malloc(sizeof(uint8_t[comp * gridsz]));

	for (size_t i = 0; i < gridsz; i++) {
		uint32_t color = grid[i] ? fg : bg;
		for (size_t j = 0; j < comp; j++) {
			size_t rsh = 8 * (comp - j - 1);
			image[comp * i + j] = 0xff & (color >> rsh);
		}
	}
	stbi_write_png_to_func(writefn, &fd,
	    width, height, comp, image, width * comp);
	free(image);
}

bool
isrgb(uint32_t x)
{
	return (x >> 24) == 0;
}

void
die(const char *s)
{
	if (s != NULL) {
		fputs(s, stderr);
	}
	fputs(usage, stderr);
	exit(1);
}

int
main(int argc, char *argv[])
{
	srand(time(NULL));

	uint32_t fg = 0xffffff, bg = 0x000000;
	size_t width = 0, height = 0;
	size_t ox = 0, oy = 0;
	size_t step = 2;

	if (argc < 3) {
		die(NULL);
	}
	width = atoi(argv[argc - 2]);
	height = atoi(argv[argc - 1]);
	if (width == 0 || height == 0) {
		die(NULL);
	}
	if (width > MAX_WIDTH) {
		die("width is too big\n");
	}
	if (height > MAX_HEIGHT) {
		die("height is too big\n");
	}

	for (int i = 1; i < argc; i++) {
		if (!strcmp("-fg", argv[i])) {
			if (++i >= argc) {
				die("-fg requires an argument");
			}
			fg = strtol(argv[i], NULL, 16);
			if (!isrgb(fg)) {
				die("-fg color must be a hexadecimal number\n");
			}
			continue;
		}
		if (!strcmp("-bg", argv[i])) {
			if (++i >= argc) {
				die("-bg requires an argument");
			}
			bg = strtol(argv[i], NULL, 16);
			if (!isrgb(bg)) {
				die("-bg color must be a hexadecimal number\n");
			}
			continue;
		}
		if (!strcmp("-step", argv[i])) {
			if (++i >= argc) {
				die("-step requires an argument");
			}
			step = atoi(argv[i]);
			if (step < 2 || step > width - 2) {
				die("-step must be in [2, WIDTH - 2]\n");
			}
			continue;
		}
		if (!strcmp("-x-offset", argv[i])) {
			if (++i >= argc) {
				die("-x-offset requires an argument");
			}
			ox = atoi(argv[i]);
			if (ox > width) {
				die("-x-offset must be in [0, WIDTH]\n");
			}
			continue;
		}
		if (!strcmp("-y-offset", argv[i])) {
			if (++i >= argc) {
				die("-y-offset requires an argument");
			}
			oy = atoi(argv[i]);
			if (oy > height) {
				die("-y-offset must be in [0, HEIGHT]\n");
			}
			continue;
		}
	}

	int fd = 1;
	if (isatty(fd)) {
		fd = open("wpgen.png", O_RDWR | O_CREAT | O_TRUNC, 0666);
	}

	struct maze m;
	m.grid = calloc(width * height, sizeof(bool));
	m.width = width;
	m.height = height;
	m.step = step;
	m.ox = ox;
	m.oy = oy;

	maze_fill(&m);
	run(fd, fg, bg, m.grid, width, height);
	free(m.grid);
	return 0;
}
