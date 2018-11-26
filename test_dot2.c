#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define DOT_FILE_NAME	"/dev/dot_driver"

int main(int argc, char **argv)
{
	int	i,j = 0;
	int	dot_fd;
	char	data;
	char	dot_values[10][9]  ={{UON,UON,UON,UON,UON,UON,UON,UFF},
				     {UON,UON,UON,UON,UON,UON,UFF,UFF},
			             {UON,UON,UON,UON,UON,UFF,UFF,UFF},
			             {UON,UON,UON,UON,UFF,UFF,UFF,UFF},
				     {UON,UON,UON,UFF,UFF,UFF,UFF,UFF},
				     {UON,UON,UFF,UFF,UFF,UFF,UFF,UFF},
				     {UON,UFF,UFF,UFF,UFF,UFF,UFF,UFF},
				     {UFF,UFF,UFF,UFF,UFF,UFF,UFF,UFF},
				     {UON,UON,UON,UON,UON,UON,UON,UON}	}; 	// Off
	
	dot_fd = open(dot_FILE_NAME, O_RDWR);
	if (dot_fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", DOT_FILE_NAME);
		return -1;
	}

	while (1)
	{

		write(dot_fd, &dot_values[i][j], sizeof(char));
		i++;
		j++;
	}

	close(dot_fd);
	return 0;
}

