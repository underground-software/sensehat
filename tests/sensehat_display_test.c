#include <err.h>
#include <stdio.h>
#include <stdint.h>

static void set(FILE *fp, uint16_t pix)
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
