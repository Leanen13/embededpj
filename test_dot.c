#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define DOT_FILE_NAME	"/dev/dot_driver"
#define UON 0x00
#define UFF 0xFF
int main(int argc, char **argv)
{
	int	i,j = 0;
	int	dot_fd;
	char	data;
	char	dot_values[9][9]  ={{UON,UON,UON,UON,UON,UON,UON,UFF},
				     {UON,UON,UON,UON,UON,UON,UFF,UFF},
			             {UON,UON,UON,UON,UON,UFF,UFF,UFF},
			             {UON,UON,UON,UON,UFF,UFF,UFF,UFF},
				     {UON,UON,UON,UFF,UFF,UFF,UFF,UFF},
				     {UON,UON,UFF,UFF,UFF,UFF,UFF,UFF},
				     {UON,UFF,UFF,UFF,UFF,UFF,UFF,UFF},
				     {UFF,UFF,UFF,UFF,UFF,UFF,UFF,UFF},
				     {UON,UON,UON,UON,UON,UON,UON,UON}	}; 	// Off
	
	dot_fd = open(DOT_FILE_NAME, O_RDWR);
	if (dot_fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", DOT_FILE_NAME);
		return -1;
	}
	

	write(dot_fd, &dot_values, sizeof(char));




	

	close(dot_fd);
	return 0;
}

