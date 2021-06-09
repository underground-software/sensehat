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

int main(int argc, char **argv)
{
	//pointer to 8x8 array of Colors that will hold the memory mapped frame buffer
	Color (*fb)[8][8]=NULL;

	//setup / argc processing code that sets up memory mapping
	{
		char *filename = "/dev/fb1";

		if(2 <= argc)
			filename = argv[1];

		int fd = open(filename, O_RDWR);
		if(-1 == fd)
			err(1, "unable to open %s for writing", filename);

		fb = mmap(NULL, sizeof *fb, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, fd, 0);
		if((void *)-1 == fb)
			err(1, "unable to mmap file %s", filename);

		close(fd);
	}

	//clear screen
	memset(*fb,0,sizeof *fb);

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
		(*fb)[loc.y][loc.x]=(Color){(uint16_t)rand()};

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
		(*fb)[loc.y][loc.x]=(Color){0};

		//update locatation
		loc = newloc;
	}

	endwin();
	munmap(fb, sizeof *fb);
	return 0;
}
