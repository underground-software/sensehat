#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <err.h>

#define _BSD_SOURCE
#define SCREEN_SIZE 8


typedef union
{
	uint16_t combined;
	struct
	{
		uint16_t r:5,g:6,b:5;
	};
}
Color;

static Color screen[SCREEN_SIZE][SCREEN_SIZE];

static int disp_fd;

static FILE *image_fd, *image_fd1;

static void update_screen(void);

static void set_screen_color(Color c, int disp);

static void draw(FILE *fd, unsigned int s, Color col, int disp);

int main(int argc, char **argv)
{
	char *disp_filename = "/dev/sense-hat";
	char *image_filename = "images/hearts.txt";
	char *image_filename1 = "images/surprised-jellyfish.txt";
	switch(argc)
	{
	case 3:
		image_filename1 = argv[2];
		[[fallthrough]];
	case 2:
		image_filename = argv[1];
		break;
	case 1:
		break;
	default:
		errx(1, "invalid arguments");
	}
	if(0 > (disp_fd = open(disp_filename, O_RDWR)))
		err(1, "unable to open %s for writing", disp_filename);
	image_fd = fopen(image_filename, "r");
	if(image_fd == NULL)
		err(1, "unable to open %s for writing", image_filename);
	image_fd1 = fopen(image_filename1, "r");
	if(image_fd1 == NULL)
		err(1, "unable to open %s for writing", image_filename1);


	Color c = {0};

	draw(image_fd, 1, c, disp_fd);
	draw(image_fd1, 1, c, disp_fd);
	draw(image_fd, 1, c, disp_fd);
	draw(image_fd, 1, c, disp_fd);

	printf("draw termintated\n");
	memset(screen, 0, sizeof screen);
	printf("memset terminated\n");
	update_screen();
	printf("written to sensehat\n");

	fclose(image_fd);
	fclose(image_fd1);
	close(disp_fd);
	printf("both files closed\n");
	return 0;
}

typedef struct
{
	uint8_t x:3,y:3;
}
Point;

static void set_pixel(Point loc, Color col)
{
	screen[loc.y][loc.x] = col;
}

void update_screen(void)
{
	if(0 > write(disp_fd, screen, sizeof screen))
		err(1, "unable to write to fb");
	if(0 > lseek(disp_fd, 0, SEEK_SET))
		err(1, "unable to seek fb");
}

void set_screen_color(Color c, int disp){
	Point loc;
	lseek(disp, 0, SEEK_SET);
	for(uint8_t x = 0; x < SCREEN_SIZE; x++){
		loc.x = x;
		for(uint8_t y = 0; y < SCREEN_SIZE; y++){
			loc.y = y;
			set_pixel(loc, c);
			update_screen();
		}
	}
	lseek(disp, 0, SEEK_SET);
}


void draw(FILE *fd, unsigned int s, Color col, int disp){
	fseek(fd, 0, SEEK_SET);
	set_screen_color(col, disp);
	Point loc = {0,0};
	int c = 0;
	Color pink = {0xFA87};
	Color white = {0};
	while(c != EOF){
		c = fgetc(fd);
		if(c == '\n')
			continue;
		switch(c){
			case 'O':
				set_pixel(loc, pink);
				break;
			case '_':
				set_pixel(loc, white);
				break;
			default:
				err(1, "Invalid char in .txt. file\n");
		}
		update_screen();
		if(loc.x == (SCREEN_SIZE - 1)){
			if(loc.y == (SCREEN_SIZE - 1)){
				break;
			}
			loc.x = 0;
			loc.y++;
			continue;
		}
		loc.x++;
	}
	sleep(s);
}
