#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define	SVM_FILE_NAME	"/dev/svm_driver"

int main(int argc, char **argv)
{
	int	fd;
	char	data;

	fd = open(SVM_FILE_NAME, O_RDWR);
	if (fd < 0)
	{
		fprintf(stderr, "Can't open %s\n", LED_FILE_NAME);
		return -1;
	}

	while (1)
	{
		fputs("select c, r, l, q : ",stdout);
    scanf("%c",&data)
    getchar();
		write(fd, &data, sizeof(char));
	}
	close(fd);
	return 0;
}
