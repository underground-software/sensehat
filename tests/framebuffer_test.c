#include <sys/mman.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <err.h>

static uint8_t screen[8][3][8];

static int fd;

static void draw_loop(void);
static void update_screen(void);

int main(int argc, char **argv)
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

	initscr();
	raw();
	noecho();
	nonl();
	intrflush(stdscr,FALSE);
	keypad(stdscr,TRUE);

	draw_loop();

	memset(screen, 0, sizeof screen);
	update_screen();

	endwin();
	close(fd);
	return 0;
}

void update_screen(void)
{
	if(0 > write(fd, screen, sizeof screen))
		err(1, "unable to write to fb");
	if(0 > lseek(fd, 0, SEEK_SET))
		err(1, "unable to seek fb");
}

typedef struct
{
	uint8_t x:3,y:3;
}
Point;

static void clear_pixel(Point loc)
{
	screen[loc.y][0][loc.x]=0;
	screen[loc.y][1][loc.x]=0;
	screen[loc.y][2][loc.x]=0;
}

static void set_pixel(Point loc, uint8_t r, uint8_t g, uint8_t b)
{
	screen[loc.y][0][loc.x]=r;
	screen[loc.y][1][loc.x]=g;
	screen[loc.y][2][loc.x]=b;
}


void draw_loop(void)
{
	Point loc = {0,0};
	for(;;)
	{
		set_pixel(loc, rand(), rand(), rand());
		update_screen();
		clear_pixel(loc);

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
			return;
		}
		loc = newloc;
	}
}
