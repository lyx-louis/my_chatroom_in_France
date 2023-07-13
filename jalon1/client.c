#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<poll.h>
#define MSG_LEN 20
#define SERV_ADDR "127.0.0.1"

void echo_client(int sockfd) {
	struct pollfd poll_fd[2];
        poll_fd[0].fd=0;
       poll_fd[0].events=POLLIN;
	   poll_fd[1].fd = sockfd;
	   poll_fd[1].events=POLLIN;
	   poll_fd[1].revents = 0;
	char buff[MSG_LEN];	
	int n;
	while (1) {
		// poll stdin
		int ret=poll(poll_fd,2,-1);
		//poll模型的本质就是轮训, 在pull返回时，轮询所有的文件描述符, 查找到有事情请求的那个文件
		if(ret<0)
          {
              perror("poll");
              continue;
          }	

			if(poll_fd[0].revents & POLLIN){
				// Cleaning memory
				memset(buff, 0, MSG_LEN);
				// Getting message from client
				printf("Message: ");
				n = 0;
				while ((buff[n++] = getchar()) != '\n') {} // trailing '\n' will be sent
				// Sending message (ECHO)
				if (send(sockfd, buff, strlen(buff), 0) <= 0) {
					break;
				}
				printf("Message sent!\n");
				if(buff[0] == '/'&&buff[1]=='q'&&buff[2]=='u'&&buff[3]=='i'&&buff[4]=='t'&&buff[5]=='\n'){
					printf("goodbye, see u next time ;)\n");
					return;
				}
			
			}

            else if( poll_fd[1].revents & POLLIN){
						// Cleaning memory
				printf("%d\n",sockfd);
				memset(buff, 0, MSG_LEN);
				if (recv(sockfd, buff, MSG_LEN, 0) <= 0) {
					printf("erreur\n");
					break;
				}
				// Receiving message
				printf("Received: %s", buff);
				poll_fd[0].revents = 1;
			}
		
		
	}
}

int handle_connect(char * port) {
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(SERV_ADDR, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}

int main(int argc, char *argv[]){
	int sfd;
	
	char *port = argv[1];
	sfd = handle_connect(port);
	echo_client(sfd);
	close(sfd);
	printf("client close\n");
	return EXIT_SUCCESS;
}