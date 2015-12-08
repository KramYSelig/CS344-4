#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd,
		portno,
		n,
		count = 0,
		fileCheck = 0,
		ptFileLength = 0,
		keyFileLength = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char c;
	char buffer[70000];
	FILE *ptfp,
		 *keyfp;

	if (argc < 4) {
		fprintf(stderr, "Usage: %s plaintext key port\n", argv[0]);
		exit(1);
	}
	fileCheck = access(argv[1], R_OK);
	if (fileCheck != 0) {
		fprintf(stderr, "Error: plaintext file is not readable!\n");
		exit(1);
	}
	fileCheck = access(argv[2], R_OK);
	if (fileCheck != 0) {
		fprintf(stderr, "Error: key file is not readable!\n");
		exit(1);
	}
 
	portno = atoi(argv[3]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname("localhost");
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	// compare size to ensure the key is at least as large as the plaintext
	ptfp = fopen(argv[1], "r");
	fseek(ptfp, 0L, SEEK_END);
	ptFileLength = ftell(ptfp);
	fseek(ptfp, 0L, SEEK_SET);
	keyfp = fopen(argv[2], "r"); 
	fseek(keyfp, 0L, SEEK_END);
	keyFileLength = ftell(keyfp);
	fseek(keyfp, 0L, SEEK_SET);
	// compare size to ensure the key is at least as large as the plaintext
	if (ptFileLength > keyFileLength) {
		fprintf(stderr, "ERROR, key must be as long as plaintext\n");
		exit(1);
	}

	count = 0;
	bzero(buffer, 70000);
	while ((c = fgetc(ptfp)) != EOF) {
		if (c != '\n') {
			buffer[count] = c;
			count++;
			c = fgetc(keyfp);
			buffer[count] = c;
			count++;
		}
	}
	n = write(sockfd, buffer, count);
	if (n < 0)
		error("ERROR writing to socket");

	bzero(buffer, 70000);
	n = read(sockfd, buffer, 255);
	printf("%s\n", buffer);
	if (n < 0)
		error("ERROR reading from socket");

	close(sockfd);
	
	return 0;
}
