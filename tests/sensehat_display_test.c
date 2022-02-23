#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>

static void set(FILE *fp, uint8_t rgb[3])
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

	char ***msg = (char**[])
	{
		(char *[]){"red1","red2","red3","red4","red5",},
		(char *[]){"green1","green2","green3","green4","green5",},
		(char *[]){"blue1","blue2","blue3","blue4","blue5",},
	};
	for(int i = 0; i < 3; ++i)
	{
		uint8_t channels[3] = {0};
		channels[i] = 1;
		for(int j = 0; j < 5; ++j)
		{
			set(fp, channels);
			puts(msg[i][j]);
			channels[i]<<=1;
			getchar();
		}
	}
	set(fp,(uint8_t[]){0,0,0});
	puts("blank");
	getchar();
	fclose(fp);
	return 0;
}
