#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
	perror(msg);
	exit(1);
}

char getEncryptedValue(char c[2]) {
	int pt = 0,
		key = 0,
		result;
	if (c[0] != 32) {
		pt = c[0] - 64;
	} else {
		pt = 0;
	}
	if (c[1] != 32) {
		key = c[1] - 64;
	} else {
		key = 0;
	}
	result = ((pt + key) % 27);
	if (result == 0) {
		result = 32;
	} else {
		result += 64;
	}
	return result;
}

int main(int argc, char *argv[]) {
	int sockfd,
		newsockfd,
		portno,
		i = 0,
		returnBufferCount = 0;
	socklen_t clilen;
	char buffer[256];
	char returnBuffer[70000];
	struct sockaddr_in serv_addr,
					   cli_addr;
	FILE *ctfp;
	int n,
		count = 0,
		buffSize = 0;
	char c1,
		 c2,
		 c3[2];

	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			error("ERROR on binding");

	listen(sockfd, 5);
	
	clilen = sizeof(cli_addr);
	
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");

	count = 0;
	n = read(newsockfd, buffer, 255);

	bzero(returnBuffer, 70000);
	returnBufferCount = 0;
	for (i = 0; i < n; i++) {
		if (i % 2 == 0) {
			c3[0] = buffer[i];
			c3[1] = buffer[i + 1];
			returnBuffer[returnBufferCount] = getEncryptedValue(c3);
			returnBufferCount++;
		}
	}
	
	if (n < 0)
		error("ERROR reading from socket");
	
	n = write(newsockfd, returnBuffer, 255);

	if (n < 0)
		error("ERROR writing to socket");
	
	close(newsockfd);
	close(sockfd);

	return 0;
}
