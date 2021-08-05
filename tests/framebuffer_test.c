#include <sys/mman.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <err.h>

typedef union
{
	uint16_t agregate;
	uint8_t bytes[2];
	struct
	{
		uint16_t blue:5;
		uint16_t green:6;
		uint16_t red:5;
	};
}
Color;

typedef struct
{
	uint8_t x:3,y:3;
}
Point;

void update_screen(int fd, void *data, size_t size)
{
	if(0 > write(fd, data, size))
		err(1, "unable to write to fb");
	if(0 > lseek(fd, 0, SEEK_SET))
		err(1, "unable to seek fb");
}

int main(int argc, char **argv)
{
	//8x8 array of Colors for holding state of frame buffer
	Color fb[8][8];

	int fd;

	//setup / argc processing code that sets up memory mapping
	{
		char *filename;

		switch(argc)
		{
		case 1:
			filename = "/dev/sense-hat";
			break;
		case 2:
			filename = argv[1];
			break;
		default:
			errx(1, "invalid arguments");
		}
		if(0 > (fd = open(filename, O_RDWR)))
			err(1, "unable to open %s for writing", filename);
	}

	//clear screen
	memset(*fb, 0, sizeof fb);
	update_screen(fd, fb, sizeof fb);

	//ncurses setup
	initscr();
	raw();
	noecho();
	nonl();
	intrflush(stdscr,FALSE);
	keypad(stdscr,TRUE);

	Point loc = {0,0};
	for(_Bool running = 1;running;)
	{
		//set current location to random color
		fb[loc.y][loc.x]=(Color){(uint16_t)rand()};
		update_screen(fd, fb, sizeof fb);

		Point newloc = loc;
		switch(getch())
		{
		case KEY_UP:
			--newloc.y;
			break;
		case KEY_DOWN:
			++newloc.y;
			break;
		case KEY_LEFT:
			--newloc.x;
			break;
		case KEY_RIGHT:
			++newloc.x;
			break;
		case '\r':
			running = 0;
			break;
		}

		//clear old location
		fb[loc.y][loc.x]=(Color){0};

		//update locatation
		loc = newloc;

		update_screen(fd, fb, sizeof fb);
	}

	endwin();
	close(fd);
	return 0;
}
