#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define SENSE_HAT_FB_FBIOGET_GAMMA 61696
#define SENSE_HAT_FB_FBIOSET_GAMMA 61697
#define SENSE_HAT_FB_FBIORESET_GAMMA 61698

static const uint8_t gamma[] = {
	000,001,002,003,004,005,006,007,
	010,011,012,013,014,015,016,017,
	020,021,022,023,024,025,026,027,
	030,031,032,033,034,035,036,037,
};

void set(FILE *fp, uint8_t rgb[3])
{
	rewind(fp);
	for(size_t i = 0; i < 8; ++i)
		for(size_t c = 0; c < 3; ++c)
			for(size_t j = 0; j < 8; ++j)
				fputc(rgb[c], fp);
	fflush(fp);
}

int main(int argc, char **argv)
{
	FILE *fp;
	{
		char *path;
		switch(argc)
		{
		case 1:
			path = "/dev/sense-hat";
			break;
		case 2:
			path = argv[1];
			break;
		default:
			errx(1,"invalid arguments");
		}
		if(NULL == (fp = fopen(path,"w")))
			err(1,"unable to open %s", path);
	}
	if(0>ioctl(fileno(fp),SENSE_HAT_FB_FBIOSET_GAMMA,gamma))
		err(1,"unable to ioctl");

	char ***msg = (char**[])
	{
		(char *[]){"red1","red2","red3","red4","red5",},
		(char *[]){"green1","green2","green3","green4","green5",},
		(char *[]){"blue1","blue2","blue3","blue4","blue5",},
	};
	for(int i = 0; i < 3; ++i)
	{
		uint8_t channels[3] = {0};
		for(int j = 0; j < 5; ++j)
		{
			channels[i] = 1 << j;
			set(fp, channels);
			puts(msg[i][j]);
			getchar();
		}
	}
	set(fp,(uint8_t[]){0,0,0});
	puts("blank");
	getchar();
	fclose(fp);
	return 0;
}
