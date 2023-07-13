#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<poll.h>
#include <stdlib.h>
//include <winsock2.h>
#include "msg_struct.h"
#define MSG_LEN 1024
#define SERV_ADDR "127.0.0.1"
#define NOM_MAX 20
#define CLIENT_COM_PORT "8081"
#define CLIENT_COM_ADDR "127.0.0.1"
#include <err.h>


void myrecv(int sockfd,char * buff,int size){
    if (recv(sockfd, buff, size, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
                        exit(EXIT_FAILURE);
	}
}

int handle_connect( char * ip,char * port) {  
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; 
	//F_UNSPEC则意味着函数返回的是适用于指定主机名和服务名且适合任何协议族的地址。如果某个主机既有AAAA记录(IPV6)地址，
	//同时又有A记录(IPV4)地址，那么AAAA记录将作为sockaddr_in6结构返回，而A记录则作为sockaddr_in结构返回
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(ip, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			printf("connect problem\n");
            perror("connect()");
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

void sendfile(int sfde,char * filename,struct message msgstruct1){
                        FILE *fp = fopen(filename, "rb");
                        char buff1[MSG_LEN]= {0} ; // 缓冲区 
                        msgstruct1.type = FILE_SEND;
                        strcpy(msgstruct1.infos,filename);
                        
                        if (send(sfde, &msgstruct1, sizeof(msgstruct1), 0) <= 0) {
							printf("send 2 client recepteur erreur\n");
                            return;
						}
                         printf("finir de send msgstruct\n");
                        int nCount;
                            while( (nCount = fread(buff1, 1, MSG_LEN, fp)) > 0 ){
                                send(sfde, buff1, nCount, 0);
                            }
                            shutdown(sfde, 1); 
                        printf("finir de send,%s\n",buff1);
                        fclose(fp);
}


	void ServerReturn(struct message msgstruct,char * buff,int sockfd,struct pollfd pollfd0,char * myname, int* salon,int* fileflags, 
    char * filename,char * userenvonyer,char * userreceiveIP, char * userreceiveport)
{
			memset(buff, 0, MSG_LEN);
				if (recv(sockfd, &msgstruct, sizeof(struct message), 0) <= 0) {
					perror("recv erreur");
                    exit(EXIT_FAILURE);
				}
				if(msgstruct.type == ERREUR_NICKNAME_NEW){
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
					printf("rece errer\n");
					perror("recv erreur");
                    exit(EXIT_FAILURE);
					}
					printf("Received: %s", buff);
					memset(myname,'\0',NOM_MAX*sizeof(char));
					printf("cant set your name, retry\n");
					
				}
				else if(msgstruct.type == ERREUR_FIND_SALON){
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
                        exit(EXIT_FAILURE);
					}
					printf("Received: %s", buff);
				}
				else if(msgstruct.type == ERREUR_CREATE_SALON){
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
                        exit(EXIT_FAILURE);
					}
					*salon = 0;
					printf("Received: %s", buff);
				}
                else if(msgstruct.type == FILE_REQUEST){
                    if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
                        exit(EXIT_FAILURE);
                    }
                    printf("User: [%s] want to send you a file named [%s], accept?[Y/N]\n", msgstruct.infos,buff);
                   *fileflags = 1;
                   strcpy(filename,buff);
                   strcat(filename,"cp");
                   strcpy(userenvonyer,msgstruct.infos);   
                   //printf("user is ,%s\n",msgstruct.infos);            
                }
                else if(msgstruct.type == FILE_ACCEPT){
                    if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
                        exit(EXIT_FAILURE);
					}
                    memset(userreceiveIP,'\0',16);
                    memset(userreceiveport,'\0',6);
                  //  char ip[16];
                    int i= 0;

                        while(buff[i]!=':'){
                            i++;
                        }
                        memcpy(userreceiveIP,buff,i);
                        memcpy(userreceiveport,buff+i+1,strlen(buff)-i);
                        printf("ip is %s, %ld\n",userreceiveIP,strlen(userreceiveIP));
                        printf("port is %s,%ld\n",userreceiveport,strlen(userreceiveport));
                        int sfde;
                        
                        sleep(1);
                        sfde = handle_connect(userreceiveIP,userreceiveport);

                         memset(userenvonyer,'\0',NOM_MAX);
                        strcpy(userenvonyer,msgstruct.infos);
                        struct message msgstruct1;
                        memset(&msgstruct1, 0, sizeof(struct message));
 
 
                        sendfile(sfde,filename,msgstruct1);

                        struct pollfd poll_fd[1];
                        	   poll_fd[0].fd = sfde;
	                            poll_fd[0].events=POLLIN;
	                           poll_fd[0].revents = 0;
                               
                            while(1){
                                int ret=poll(poll_fd,1,-1);
                                if(ret<0)
                                {
                                    perror("poll");
                                    continue;
                                }	
                                if(poll_fd[0].revents & POLLIN){
                                    if (recv(sfde, &msgstruct1, sizeof(struct message), 0) <= 0) {
                                        perror("recv erreur");
                                        exit(EXIT_FAILURE);
                                     }
                                    if(msgstruct1.type == FILE_ACK){
                                        printf("%s has revceived the file\n",userenvonyer);
                                        int i;
                                        i = close(sfde);
                                        poll_fd[0].fd = -1;
                    					poll_fd[0].events = 0;
					                    poll_fd[0].revents = 0;
                                        
                                        if(i == 0){
                                            printf("close reussir\n");
                                            break;
                                        }
                                        else{
                                            printf("close erreur\n");
                                            perror("close()");
                                        }
                                    } 
                                }
                            }       
                        
                }
				else{
					if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
						printf("rece errer\n");
						perror("recv erreur");
                        exit(EXIT_FAILURE);
					}
					printf("Received: %s", buff);
				} //
				pollfd0.revents = 1;
              
}

void buffnick(char * buff,struct message msgstruct,int sockfd,char *myname){
    if(buff[0]=='/'&&buff[1]=='n'&&buff[2]=='i'&&buff[3]=='c'&&buff[4]=='k'&&buff[5]==' '&&strlen(buff)>6&&strlen(buff)<(NOM_MAX+6)){
						memcpy(myname,buff+6,strlen(buff)-7);
						printf("first set my name is %s\n",myname);
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
void buffrenick(char * buff,struct message msgstruct,int sockfd,char *myname){
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

void buffwho(int sockfd,struct message msgstruct){
                            msgstruct.type = NICKNAME_LIST;
                            if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                printf("send err\n");
                                return;
                            }
                            printf("send!\n");
}
void buffwhois(int sockfd,struct message msgstruct,char* buff){
                        msgstruct.type = NICKNAME_INFOS;
                        memcpy(msgstruct.infos,buff+7,strlen(buff)-8);
                        if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                return;
                            }
}
void buffmsgall(int sockfd,struct message msgstruct,char* buff){
                        msgstruct.type = BROADCAST_SEND;

                        char* listen=(char *)malloc(MSG_LEN);
					    memset(listen,'\0',MSG_LEN);
                        memcpy(listen,buff+8,strlen(buff)-9);
                        msgstruct.pld_len = strlen(buff);
                        if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                exit(EXIT_FAILURE);
                            }
                        if (send(sockfd, listen, msgstruct.pld_len, 0) <= 0) {
                        printf("send msgall error\n");
                        exit(EXIT_FAILURE);
                        }
                        free(listen);
}

void buffmsg(int sockfd,struct message msgstruct,char* buff){
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
                                exit(EXIT_FAILURE);
                            }
                        if (send(sockfd, contenu, msgstruct.pld_len, 0) <= 0) {
                        printf("send msg error\n");
                        exit(EXIT_FAILURE);
                        }
                        free(sousbuff);
                        free(namemsg);
                        free(contenu);
}
void buffcreate(int sockfd,struct message msgstruct,char* buff,int * salon){
                        msgstruct.type = MULTICAST_CREATE;
                        *salon = 1;
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

void buffchannellist(int sockfd,struct message msgstruct){
                          msgstruct.type = MULTICAST_LIST;
                            if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                printf("send err\n");
                                return;
                            }
                            printf("send!\n");
}

void buffjoin(int sockfd,struct message msgstruct,char* buff,int * salon){
                            msgstruct.type = MULTICAST_JOIN;
                            *salon = 1;
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


void buffquit(int sockfd,struct message msgstruct,char* buff,int * salon){
                            msgstruct.type = MULTICAST_QUIT;
                            *salon = 0;
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

void traitesend(char * buff,char * sousbuff,char * namemsg,char * contenu,int i){
                                       memset(namemsg,'\0',i+1);
                                        memcpy(namemsg,sousbuff,i);
                                       
                                        memset(contenu,'\0',strlen(buff)-i);
                                        memcpy(contenu,sousbuff+i+1,strlen(sousbuff)-i);
                                        
}

void freechar(char * sousbuff,char * namemsg,char * contenu){
    free(sousbuff);
    free(namemsg);
    free(contenu);
}

void buffsendfindfp(int sockfd,struct message msgstruct,char * namemsg, char * contenu,char * filename){
                                                strcpy(msgstruct.infos,namemsg);
                                                msgstruct.pld_len = strlen(contenu);
                                                
                                                if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                                        perror("send erreur");
                                                            exit(EXIT_FAILURE);
                                                    }
                                                if (send(sockfd, contenu, msgstruct.pld_len, 0) <= 0) {
                                                printf("send msg error\n");
                                                    perror("send erreur");
                                                    exit(EXIT_FAILURE);
                                                }

                                                memset(filename,'\0',MSG_LEN);
                                                strcpy(filename,contenu);

}

void buffecho_groupe(int sockfd,struct message msgstruct,char* buff,int * salon){
                            if(*salon == 0){
                            //printf("send\n");
                            msgstruct.type = ECHO_SEND;
                            msgstruct.pld_len = strlen(buff);
                            if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                perror("send erreur");
                                exit(EXIT_FAILURE);
                            }
                            if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
                            perror("send erreur");
                            exit(EXIT_FAILURE);
                            }
                        }	
                        else {
                            //printf("is salon? %d\n",*salon);
                            msgstruct.type = MULTICAST_SEND;
                            msgstruct.pld_len = strlen(buff);
                            if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                perror("send erreur");
                                exit(EXIT_FAILURE);
                            }
                            if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
                            perror("send erreur");
                            exit(EXIT_FAILURE);
                            }
                        }
}

int handle_bind()
{
                            struct addrinfo hints, *result, *rp;
                        int sfd;
                        memset(&hints, 0, sizeof(struct addrinfo));
                        hints.ai_family = AF_UNSPEC;
                        hints.ai_socktype = SOCK_STREAM;
                        hints.ai_flags = AI_PASSIVE;
                        	if (getaddrinfo(NULL, CLIENT_COM_PORT, &hints, &result) != 0) {
                                perror("getaddrinfo()");
                                exit(EXIT_FAILURE);
                            }
                            for (rp = result; rp != NULL; rp = rp->ai_next) {
                                sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
                                int yes = 1;

                                if (sfd == -1) {
                                    continue;
                                }
                                if (-1 == setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {

                                perror("setsockopt");
                                exit(EXIT_FAILURE);
                                }
                                if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
                                    break;
                                }
                                close(sfd);
                                
                            } 
                            if (rp == NULL) {
                                    fprintf(stderr, "Could not bind\n");
                                    exit(EXIT_FAILURE);
                                }
                                freeaddrinfo(result);
                            return sfd;
}

FILE * detecfichier(char * filename){
                            int te;
                        FILE * fp2;
                        te = access(filename,0);
                        if(te == 0) { //检测文件是否存在 存在则进入此循环
                            printf("fichier exist\n");
                            remove(filename);
                            fp2 = fopen(filename, "wb");
                        }
                        else{
                            printf("fichier dont existe\n");
                            fp2 = fopen(filename, "wb");
                        }
                        return fp2;
}

void buffack(int client_fd,FILE * fp2,struct message msgstruct1){
                                        char buffer[MSG_LEN] = {0};  //文件缓冲区
                                        int nCount;
                                        printf("test\n");   
                                        while( (nCount = recv(client_fd, buffer, MSG_LEN, 0)) > 0 ){
                                            fwrite(buffer, nCount, 1, fp2);
                                        }
                                        printf("File transfer success!\n");
                                        fclose(fp2); 
                                        memset(&msgstruct1, 0, sizeof(struct message));
                                        msgstruct1.type =FILE_ACK;
                                        if (send(client_fd, &msgstruct1, sizeof(msgstruct1), 0) <= 0) {
                                            printf("send 2 client envoyeur erreur\n");
                                            perror("send()");
                                            return;
                                        }

}

void buffreject(int sockfd,struct message msgstruct,char *userenvoyer, int * ptr_files){
                            msgstruct.type = FILE_REJECT;
                        *ptr_files = 0;
                        strcpy(msgstruct.infos,userenvoyer);
                        printf("refuse\n");
                         if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                perror("send()");
                                return;
                            }
}

void buffsendaccept(char * buff,char * userenvoyer,struct message msgstruct,int sockfd){
                            memset(buff,'\0',MSG_LEN);
                        sprintf(buff,"%s:%s",CLIENT_COM_ADDR,CLIENT_COM_PORT);
                        strcpy(msgstruct.infos,userenvoyer);
                        msgstruct.pld_len = strlen(buff);
                        if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                return;
                            }
                        if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
                            printf("send msg error\n");
                                exit(EXIT_FAILURE);
                            }
                        printf("send accept!\n");
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
	int salon = 0;
    int * ptr_salon;
    ptr_salon = &salon;
    int fileflags = 0;
    int * ptr_files;
    ptr_files = &fileflags;
	char filename[MSG_LEN];
    char userenvoyer[NOM_MAX];
    char userreceiveIP[16];
    char userreceiveport[6];
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
        memset(&msgstruct, 0, sizeof(struct message));
		memset(buff, 0, MSG_LEN);
		if(poll_fd[0].revents & POLLIN && strlen(myname)==0){
					n = 0;
                    memset(&msgstruct, 0, sizeof(struct message));
					while ((buff[n++] = getchar()) != '\n') {} 
                    buffnick(buff,msgstruct,sockfd,myname);
			}
		else if(poll_fd[0].revents & POLLIN && strlen(myname)!=0){
					n = 0;
				while ((buff[n++] = getchar()) != '\n') {} 
				if(fileflags == 0){
                    if(buff[0]=='/'&&buff[1]=='n'&&buff[2]=='i'&&buff[3]=='c'&&buff[4]=='k'&&buff[5]==' '&&strlen(buff)>6&&strlen(buff)<(NOM_MAX+6)){
                            buffrenick(buff,msgstruct,sockfd,myname);
                    }
                    else if(buff[0]=='/'&&buff[1]=='n'&&buff[2]=='i'&&buff[3]=='c'&&buff[4]=='k'&&buff[5]==' '&&strlen(buff)>(NOM_MAX+6)){
					        printf("name maximal 20 char,retry\n");
				    }
                    else if(buff[0]=='/'&&buff[1]=='w'&&buff[2]=='h'&&buff[3]=='o'&&strlen(buff)==5){
                            buffwho(sockfd,msgstruct);
                    }
                    else if(buff[0]=='/'&&buff[1]=='w'&&buff[2]=='h'&&buff[3]=='o'&&buff[4]=='i'&& buff[5]=='s'&&buff[6]==' '&&strlen(buff)>8){
                        buffwhois(sockfd,msgstruct,buff);
                    }
                    else if(buff[0]=='/'&&buff[1]=='m'&&buff[2]=='s'&&buff[3]=='g'&&buff[4]=='a'&& buff[5]=='l'&&buff[6]=='l'&&buff[7]==' '&&strlen(buff)>9){
                        buffmsgall(sockfd,msgstruct,buff);
                    }
                    else if(buff[0]=='/'&&buff[1]=='m'&&buff[2]=='s'&&buff[3]=='g'&&buff[4]==' '&&strlen(buff)>5){
                            buffmsg(sockfd,msgstruct,buff);
                    }
                    else if(buff[0]=='/'&&buff[1]=='q'&&buff[2]=='u'&&buff[3]=='i'&&buff[4]=='t'&&buff[5]=='\n'){
					        printf("goodbye! see u next time\n");
					        return;
				    }
                    else if(buff[0]=='/'&&buff[1]=='c'&&buff[2]=='r'&&buff[3]=='e'&&buff[4]=='a'&&buff[5]=='t'&&buff[6]=='e'&&buff[7]==' '&&strlen(buff)>9&&strlen(buff)<=NOM_MAX+9) {
                            buffcreate(sockfd,msgstruct,buff,ptr_salon);
                    }
                    else if(buff[0]=='/'&&buff[1]=='c'&&buff[2]=='r'&&buff[3]=='e'&&buff[4]=='a'&&buff[5]=='t'&&buff[6]=='e'&&buff[7]==' '&&strlen(buff)>NOM_MAX+9){
					        printf("name maximal 20 char,retry\n");
				    }
                    else if(buff[0]=='/'&&buff[1]=='c'&&buff[2]=='h'&&buff[3]=='a'&&buff[4]=='n'&&buff[5]=='n'&&buff[6]=='e'&&buff[7]=='l'
                                && buff[8]=='_'&& buff[9]=='l'&& buff[10]=='i'&& buff[11]=='s'&& buff[12]=='t'&&strlen(buff)==14){
                             buffchannellist(sockfd,msgstruct);
                    }
                    else if(buff[0]=='/'&&buff[1]=='j'&&buff[2]=='o'&&buff[3]=='i'&&buff[4]=='n'&& buff[5] == ' '&&strlen(buff)>6){
                            buffjoin(sockfd,msgstruct,buff,ptr_salon);
                    }
                    else if(buff[0]=='/'&&buff[1]=='q'&&buff[2]=='u'&&buff[3]=='i'&&buff[4]=='t'&& buff[5] == ' '&&strlen(buff)>6){
                            buffquit(sockfd,msgstruct,buff,ptr_salon);
                    }
                    else if(buff[0]=='/'&&buff[1]=='s'&&buff[2]=='e'&&buff[3]=='n'&&buff[4]=='d'&& buff[5] == ' '&&strlen(buff)>6){
                                        msgstruct.type = FILE_REQUEST;
                                        char* sousbuff=(char *)malloc(strlen(buff)-6);
                                        memset(sousbuff,'\0',strlen(buff)-6);
                                        memcpy(sousbuff,buff+6,strlen(buff)-7);
                                        int i= 0;
                                        while(sousbuff[i]!=' '){
                                            i++;
                                        }
                                        char * namemsg = (char *)malloc(NOM_MAX);
                                        char * contenu = (char *)malloc(MSG_LEN);
                                        memset(namemsg,'\0',i+1);
                                        memcpy(namemsg,sousbuff,i);
                                        memset(contenu,'\0',strlen(buff)-i);
                                        memcpy(contenu,sousbuff+i+1,strlen(sousbuff)-i);
                                            FILE *fp = fopen(contenu, "rb");
                                            if(fp == NULL){
                                                printf("no this file, retry\n");
                                                    free(sousbuff);
                                                    free(namemsg);
                                                    free(contenu);
                                                    fclose(fp);
                                                continue;
                                            }
                                            else{
                                                buffsendfindfp(sockfd,msgstruct,namemsg,contenu,filename);
                                                    fclose(fp);
                                                    free(sousbuff);
                                                    free(namemsg);
                                                    free(contenu);
                                            }                             
                    }
                    else {
                            buffecho_groupe(sockfd,msgstruct,buff,ptr_salon);
                    }    
                }
                else if(fileflags == 1){
                    if(buff[0] == 'Y'&& buff[1]=='\n'&&strlen(buff) == 2)
                    {
                        msgstruct.type = FILE_ACCEPT;
                        *ptr_files =0;
                        buffsendaccept(buff,userenvoyer,msgstruct,sockfd);
                        FILE * fp2;
                        fp2 = detecfichier(filename);
                        struct message msgstruct1;
                        memset(&msgstruct1, 0, sizeof(struct message));
                        int sfd;
                        sfd = handle_bind();
                        if ((listen(sfd, SOMAXCONN)) != 0) {
                                    perror("listen()\n");
                                    exit(EXIT_FAILURE);
                             }  

                                int nfds = 2;
                                struct pollfd pollfds[nfds];
                                	 pollfds[0].fd = sfd;
                                    pollfds[0].events = POLLIN; // 普通或优先带数据可读
                                    pollfds[0].revents = 0;
                                    pollfds[1].fd = -1;
                                    pollfds[1].events = 0;
                                    pollfds[1].revents = 0;
                                while(1){
                                    int n_active = 0;
                                    if (-1 == (n_active = poll(pollfds, nfds, -1))) {
                                        perror("Poll");
                                    }
                                        if (pollfds[0].revents & POLLIN){
                                               struct sockaddr client_addr;
                                                socklen_t size = sizeof(client_addr);
                                                int client_fd;
                                                if (-1 == (client_fd = accept(sfd, &client_addr, &size))) {
                                                    perror("Accept");
                                                    exit(EXIT_FAILURE);
                                                }  
                                                
                                                pollfds[1].fd = client_fd;
                                                pollfds[1].events = POLLIN;
                                                pollfds[0].revents = 0;                                                                                                                                                 
                                        }
                                        else if(pollfds[1].fd != sfd && pollfds[1].revents & POLLIN){
                                                                        if (recv(pollfds[1].fd, &msgstruct1, sizeof(struct message), 0) <= 0) {
                                                                            perror("recv erreur");
                                                                            exit(EXIT_FAILURE);
                                                                        }
                                                                        if(msgstruct1.type == FILE_SEND){
                                                                                    buffack(pollfds[1].fd,fp2,msgstruct1);
                                                                                    close(pollfds[1].fd);
                                                                                    pollfds[1].fd = -1;
                                                                                    pollfds[1].events = 0;
                                                                                    pollfds[1].revents = 0;
                                                                                     close(pollfds[0].fd);
                                                                                     pollfds[0].fd = -1;
                                                                                    pollfds[0].events = 0;
                                                                                    pollfds[0].revents = 0;
                                                                                    printf("all clean\n");
                                                                                    break;
                                                                        }
                                        }                                   
                                }
                    }
                    else if(buff[0] == 'N'&& buff[1] == '\n'&&strlen(buff) == 2){
                            buffreject(sockfd,msgstruct,userenvoyer,ptr_files);
                    }
                    else{
                        printf("retry\n");
                    }

                }              
		}
		else if(/*poll_fd[i].fd == sockfd &&*/ poll_fd[1].revents & POLLIN){
				ServerReturn(msgstruct,buff,sockfd,poll_fd[0],myname,ptr_salon,ptr_files,filename,userenvoyer,userreceiveIP,userreceiveport);
			}

	}
}


int main(int argc, char *argv[]) {
	int sfd;
	char *port = argv[1];
	sfd = handle_connect(SERV_ADDR,port);
	echo_client(sfd);
	close(sfd);
	return EXIT_SUCCESS;
}

