#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <err.h>

static char const *get_syn_type(struct input_event const *evt)
{
	switch(evt->code)
	{
	case SYN_REPORT:
		return "report";
	case SYN_DROPPED:
		return "dropped";
	default:
		return "???";
	}
}

static char const *get_key_name(struct input_event const *evt)
{
	switch(evt->code)
	{
	case BTN_DPAD_UP:
		return "up";
	case BTN_DPAD_DOWN:
		return "down";
	case BTN_DPAD_LEFT:
		return "left";
	case BTN_DPAD_RIGHT:
		return "right";
	case BTN_SELECT:
		return "select";
	default:
		return "???";
	}
}

static char const *get_key_state(struct input_event const *evt)
{
	switch(evt->value)
	{
	case 0:
		return "released";
	case 1:
		return "pressed";
	case 2:
		return "held";
	default:
		return "???";
	}
}

int main(int argc, char **argv)
{
	char *filename;
	int fd;
	switch(argc)
	{
	case 1:
		filename = "/dev/input/event0";
		break;
	case 2:
		filename = argv[1];
		break;
	default:
		errx(1, "Usage: %s [optional path to joystick]", argv[0]);
	}

	if(0 > (fd = open(filename, O_RDONLY)))
		err(1, "unable to open joystick %s", filename);

	for(struct input_event evt;;)
	{
		if(0 > read(fd, &evt, sizeof evt))
		{
			warn("read returned negative");
			continue;
		}

		switch(evt.type)
		{
		case EV_SYN:
			printf("syn event(%d): %8s(%d)\n", evt.type,
				get_syn_type(&evt), evt.code);
			break;
		case EV_KEY:
			printf("key event(%d): %8s(%d)\t%8s(%d)\n", evt.type,
				get_key_name(&evt), evt.code,
				get_key_state(&evt), evt.value);
			break;
		default:
			printf("??? event(%d)\n", evt.type);
			break;
		}
	}
}
