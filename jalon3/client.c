#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<poll.h>

#include "msg_struct.h"
#define MSG_LEN 1024
#define SERV_ADDR "127.0.0.1"
#define NOM_MAX 20

	void ServerReturn(struct message msgstruct,char * buff,int sockfd,struct pollfd pollfd0,char * myname, int *salon)
{
			memset(buff, 0, MSG_LEN);
				if (recv(sockfd, &msgstruct, sizeof(struct message), 0) <= 0) {
					perror("recv erreur");
				}
				if(msgstruct.type == ERREUR_NICKNAME_NEW){
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
					printf("rece errer\n");
					perror("recv erreur");
					}
					printf("Received: %s", buff);
					memset(myname,'\0',NOM_MAX*sizeof(char));
					printf("cant set your name, retry\n");
					
				}
				else if(msgstruct.type == ERREUR_FIND_SALON){
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
					}
					printf("Received: %s", buff);
				}
				else if(msgstruct.type == ERREUR_CREATE_SALON){
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
					}
					*salon = 0;
					printf("Received: %s", buff);
				}
				else{
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
					}
					printf("Received: %s", buff);
				} //
				pollfd0.revents = 1;
}

void echo_client(int sockfd) {
	struct message msgstruct;
	struct pollfd poll_fd[2];
        poll_fd[0].fd=0;
       poll_fd[0].events=POLLIN;
	   poll_fd[1].fd = sockfd;
	   poll_fd[1].events=POLLIN;
	   poll_fd[1].revents = 0;
	int n;
	char myname[NOM_MAX];
	int salon;
	int * ptr_salon;
    ptr_salon = &salon;
	memset(myname,'\0',NOM_MAX*sizeof(char));
	printf("Connecting to server ... done!\n");
	while (1) {
		int ret=poll(poll_fd,2,-1);
		if(ret<0)
          {
              perror("poll");
              continue;
          }	
		// Cleaning memory
		char buff[MSG_LEN];
		memset(buff, 0, MSG_LEN);
		memset(&msgstruct, 0, sizeof(struct message));

		if(/*poll_fd[i].fd == 0 &&*/ poll_fd[0].revents & POLLIN && strlen(myname)==0){
					n = 0;
					while ((buff[n++] = getchar()) != '\n') {} 
					if(buff[0]=='/'&&buff[1]=='n'&&buff[2]=='i'&&buff[3]=='c'&&buff[4]=='k'&&buff[5]==' '&&strlen(buff)>6&&strlen(buff)<(NOM_MAX+6)){
						memcpy(myname,buff+6,strlen(buff)-7);
						printf("my name is %s\n",myname);
						msgstruct.type = NICKNAME_NEW;
						strcpy(msgstruct.infos,myname);
						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
						}
					}
					else{
						printf("please set your name, max 20 char\n");
					}
			}
		else if(/*poll_fd[i].fd == 0 &&*/ poll_fd[0].revents & POLLIN && strlen(myname)!=0){
			
				n = 0;
				while ((buff[n++] = getchar()) != '\n') {} 
				if(buff[0]=='/'&&buff[1]=='n'&&buff[2]=='i'&&buff[3]=='c'&&buff[4]=='k'&&buff[5]==' '&&strlen(buff)>6&&strlen(buff)<(NOM_MAX+6)){
						//memset(&msgstruct, 0, sizeof(struct message));
						msgstruct.type = NICKNAME_NEW;
						strcpy(msgstruct.nick_sender,myname);
						memset(myname,'\0',NOM_MAX*sizeof(char));
						memcpy(myname,buff+6,strlen(buff)-7);
                        printf("myname is %s\n",myname);
						strcpy(msgstruct.infos,myname);

						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
						}
				}
				else if(buff[0]=='/'&&buff[1]=='n'&&buff[2]=='i'&&buff[3]=='c'&&buff[4]=='k'&&buff[5]==' '&&strlen(buff)>(NOM_MAX+6)){
					printf("name maximal 20 char,retry\n");
				}
				else if(buff[0]=='/'&&buff[1]=='w'&&buff[2]=='h'&&buff[3]=='o'&&strlen(buff)==5){
						msgstruct.type = NICKNAME_LIST;
						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send err\n");
							return;
						}
						printf("send!\n");
				}
				else if(buff[0]=='/'&&buff[1]=='w'&&buff[2]=='h'&&buff[3]=='o'&&buff[4]=='i'&& buff[5]=='s'&&buff[6]==' '&&strlen(buff)>8){
					msgstruct.type = NICKNAME_INFOS;
					memcpy(msgstruct.infos,buff+7,strlen(buff)-8);
					if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
						}
				}

				else if(buff[0]=='/'&&buff[1]=='m'&&buff[2]=='s'&&buff[3]=='g'&&buff[4]=='a'&& buff[5]=='l'&&buff[6]=='l'&&buff[7]==' '&&strlen(buff)>9){
					msgstruct.type = BROADCAST_SEND;

					char* listen=(char *)malloc(MSG_LEN);
					memset(listen,'\0',MSG_LEN);
					memcpy(listen,buff+8,strlen(buff)-9);
					msgstruct.pld_len = strlen(buff);
					if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
						}
					if (send(sockfd, listen, msgstruct.pld_len, 0) <= 0) {
					printf("send msgall error\n");
					break;
					}

					
					free(listen);
				}
				else if(buff[0]=='/'&&buff[1]=='m'&&buff[2]=='s'&&buff[3]=='g'&&buff[4]==' '&&strlen(buff)>5){
					msgstruct.type = UNICAST_SEND;
					char* sousbuff=(char *)malloc(strlen(buff)-5);
					memset(sousbuff,'\0',strlen(buff)-5);
					memcpy(sousbuff,buff+5,strlen(buff)-6);
					int i= 0;

					while(sousbuff[i]!=' '){
						i++;
					}
					char * namemsg = (char *)malloc(i+1);
                    memset(namemsg,'\0',i+1);
					memcpy(namemsg,sousbuff,i);
					char * contenu = (char *)malloc(strlen(sousbuff)-i);
                    memset(contenu,'\0',strlen(sousbuff)-i);
					memcpy(contenu,sousbuff+i+1,strlen(sousbuff)-i);
					strcpy(msgstruct.infos,namemsg);
					msgstruct.pld_len = strlen(contenu);
					if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
						}
					if (send(sockfd, contenu, msgstruct.pld_len, 0) <= 0) {
					printf("send msg error\n");
					break;
					}
                    free(sousbuff);
                    free(namemsg);
                    free(contenu);
				}
                else if(buff[0]=='/'&&buff[1]=='c'&&buff[2]=='r'&&buff[3]=='e'&&buff[4]=='a'&&buff[5]=='t'&&buff[6]=='e'&&buff[7]==' '&&strlen(buff)>9&&strlen(buff)<=NOM_MAX+9) {
                    msgstruct.type = MULTICAST_CREATE;
					salon = 1;
                    char* listen=(char *)malloc(NOM_MAX);
					memset(listen,'\0',NOM_MAX);
					memcpy(listen,buff+8,strlen(buff)-9);
                    strcpy(msgstruct.infos,listen);
                    if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
					}
                    printf("demande creat salon send!\n");
					free(listen);
                }
				else if(buff[0]=='/'&&buff[1]=='c'&&buff[2]=='r'&&buff[3]=='e'&&buff[4]=='a'&&buff[5]=='t'&&buff[6]=='e'&&buff[7]==' '&&strlen(buff)>NOM_MAX+9){
					printf("name maximal 20 char,retry\n");
				}
				else if(buff[0]=='/'&&buff[1]=='c'&&buff[2]=='h'&&buff[3]=='a'&&buff[4]=='n'&&buff[5]=='n'&&buff[6]=='e'&&buff[7]=='l'
						&& buff[8]=='_'&& buff[9]=='l'&& buff[10]=='i'&& buff[11]=='s'&& buff[12]=='t'&&strlen(buff)==14){
							msgstruct.type = MULTICAST_LIST;
						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send err\n");
							return;
						}
						printf("send!\n");
				}
				else if(buff[0]=='/'&&buff[1]=='j'&&buff[2]=='o'&&buff[3]=='i'&&buff[4]=='n'&& buff[5] == ' '&&strlen(buff)>6){
						msgstruct.type = MULTICAST_JOIN;
						salon = 1;
						char* listen=(char *)malloc(NOM_MAX);
						memset(listen,'\0',NOM_MAX);
						memcpy(listen,buff+6,strlen(buff)-7);
						strcpy(msgstruct.infos,listen);
						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send err\n");
							return;
						}
						free(listen);
				}
				else if(buff[0]=='/'&&buff[1]=='q'&&buff[2]=='u'&&buff[3]=='i'&&buff[4]=='t'&&buff[5]=='\n'){
					printf("goodbye! see u next time\n");
					return;
				}
				else if(buff[0]=='/'&&buff[1]=='q'&&buff[2]=='u'&&buff[3]=='i'&&buff[4]=='t'&& buff[5] == ' '&&strlen(buff)>6){
						msgstruct.type = MULTICAST_QUIT;
						salon = 0;
						char* listen=(char *)malloc(NOM_MAX);
						memset(listen,'\0',NOM_MAX);
						memcpy(listen,buff+6,strlen(buff)-7);
						strcpy(msgstruct.infos,listen);
						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send err\n");
							return;
						}
						free(listen);
				}
				else {
					if(salon == 0){
						msgstruct.type = ECHO_SEND;
						msgstruct.pld_len = strlen(buff);
						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
						}
						if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("send msg error\n");
						break;
						}
					}	
					else {
						msgstruct.type = MULTICAST_SEND;
						msgstruct.pld_len = strlen(buff);
						if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							return;
						}
						if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("send msg error\n");
						break;
						}
					}
				}
		}

	
		else if(/*poll_fd[i].fd == sockfd &&*/ poll_fd[1].revents & POLLIN){
				ServerReturn(msgstruct,buff,sockfd,poll_fd[0],myname,ptr_salon);
			}

	}
}

int handle_connect( char * port) {  
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; 
	//F_UNSPEC则意味着函数返回的是适用于指定主机名和服务名且适合任何协议族的地址。如果某个主机既有AAAA记录(IPV6)地址，
	//同时又有A记录(IPV4)地址，那么AAAA记录将作为sockaddr_in6结构返回，而A记录则作为sockaddr_in结构返回
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

int main(int argc, char *argv[]) {
	int sfd;
	char *port = argv[1];
	sfd = handle_connect(port);
	echo_client(sfd);
	close(sfd);
	return EXIT_SUCCESS;
}

