#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#define	BUFSIZE	30

void error_handling(char *message);

int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[BUFSIZE];
	int str_len;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error_handling("connect() error");

	while (1)
	{
		memset(message, '\0', BUFSIZE);
		fputs("Send Message to server (q to quit ) : ", stdout);
		fgets(message, BUFSIZE, stdin);

		if (strcmp(message, "q\n") == 0) 
			break;

		write(sock, message, strlen(message));

		str_len = read(sock, message, BUFSIZE - 1);
		if (str_len == -1)
			error_handling("read() error");

		message[str_len] = 0;
		fprintf(stdout, "Message from server : %s \n", message);
	}

	close(sock);
	return 0;
}

void error_handling(char* message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}
