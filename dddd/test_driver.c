#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>
#include <time.h>
#include <unistd.h>


#define DOT_FILE_NAME   "/dev/dot_driver"
int main(int argc, char **argv)
{
	int     dot_fd;
	char    data;
	char	val;
	int     number=2;
	char    dot_values[10][8] = {
		{0xC3, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xC3},               //0
		{0xF7, 0xE7, 0xF7, 0xF7, 0xF7, 0xF7, 0xF7, 0xC7},               //1
		{0x3C, 0x04, 0x04, 0x3C, 0x20, 0x20, 0x20, 0x3C},               //2
		{0x3C, 0x04, 0x04, 0x3C, 0x04, 0x04, 0x04, 0x3C},               //3             
		{0x28, 0x28, 0x28, 0x28, 0x3C, 0x08, 0x08, 0x08},               //4
		{0x3C, 0x20, 0x20, 0x3C, 0x04, 0x04, 0x04, 0x3C},               //5
		{0x3C, 0x20, 0x20, 0x20, 0x3C, 0x24, 0x24, 0x3C},               //6
		{0x3C, 0x24, 0x24, 0x04, 0x04, 0x04, 0x04, 0x04},               //7
		{0x3C, 0x24, 0x24, 0x3C, 0x24, 0x24, 0x24, 0x3C},               //8
		{0x3C, 0x24, 0x24, 0x3C, 0x04, 0x04, 0x04, 0x3C}                //9
	};
	char    dot_char[3][8] = {
		{0x3C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08},               //t
		{0x3C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3C},               //c
		{0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00}                //:
	};

	dot_fd = open(DOT_FILE_NAME, O_RDWR);
	if (dot_fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", DOT_FILE_NAME);
		return -1;
	}
	
	while (1)
	{
		write(dot_fd, dot_values[number], sizeof(char));
	}

	close(dot_fd);
	return 0;
}

