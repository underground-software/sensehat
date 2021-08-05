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

void set(FILE *fp, uint16_t pix)
{
	rewind(fp);
	for(size_t i = 0; i < 64; ++i)
	{
		fputc(pix&0xff,fp);
		fputc(pix>>8, fp);
	}
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

	char **msg = (char*[]){	"blue1","blue2","blue3","blue4","blue5","blank",
				"green1","green2","green3","green4","green5",
				"red1","red2","red3","red4","red5","blank",};
	for(uint16_t mask = 1; mask != 0; mask <<= 1)
	{
		set(fp,mask);
		puts(*msg++);
		getchar();
	}
	set(fp,0);
	puts(*msg++);
	getchar();
	fclose(fp);
	return 0;
}
