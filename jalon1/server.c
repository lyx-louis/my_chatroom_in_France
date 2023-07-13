#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#define MSG_LEN 20
// struct liste chaine
typedef struct chaineclient{
	int fd;
	int port;
	struct chaineclient *prev;
	char * ip;

	struct chaineclient *next;
} LinkList;

// free 
void delete_chain(LinkList *head)
{
	LinkList *p1;
	while (head)
	{
		p1 = head;
		head = head->next;
		
		free(p1->ip);
		
		free(p1);
	}
}

LinkList * delete_node(LinkList * head, int fd){
    LinkList * temp = NULL;
	temp = head;
    while(temp->fd!= fd){
        temp = temp->next;
        if(temp == NULL){
            printf("[Warning]: no this port to delete");
            exit(EXIT_FAILURE);
        }
    }
    
    if(temp->next != NULL&&temp->prev !=NULL){
        LinkList * temp_prev = temp->prev;
        LinkList * temp_next = temp->next;
        temp_prev->next = temp_next;
        temp_next->prev = temp_prev;
    }
    else if(temp->next == NULL){
        LinkList * temp_prev = temp->prev;
        temp_prev->next = NULL;
    }
    free(temp->ip);
    //free(temp->nickname);
    free(temp);
    return head;
}
LinkList *InsertElem(LinkList * p,LinkList * elem){
    LinkList * temp = p;
    while(temp->next != NULL){
        temp = temp->next;
    }
    
    elem->next = NULL;
    elem->prev = temp;
    temp->next = elem;
    return p;
}

void echo_server(int sockfd) {
     int nfds = 10;
     struct pollfd pollfds[nfds];
     // Init first slot with listening socket
	 pollfds[0].fd = sockfd;
	 pollfds[0].events = POLLIN; // 普通或优先带数据可读
	 pollfds[0].revents = 0;
	 int i;
	 // Init remaining slot to default values
	 for (i = 1; i < nfds; i++) {
	 	pollfds[i].fd = -1;
	 	pollfds[i].events = 0;
	 	pollfds[i].revents = 0;
	 }
	LinkList *head, *node;
	head = (LinkList*)malloc(sizeof(LinkList));
	//end = head;
	 head->next = NULL;
	 head->ip = (char *)malloc(16);
	 head->fd = sockfd;
	memset(head->ip,'\0',10*sizeof(char));
	char buff[MSG_LEN];
	int fd_active = 0;
	while (1) {
         int n_active = 0;
		 if (-1 == (n_active = poll(pollfds, nfds, -1))) {
		 	perror("Poll");
		 }
		// printf("[SERVER] : %d active socket\n", n_active); // 应该只有当前的激活的socket才对
		 


		
		
        for (i=0; i < nfds; i++){
			
            	if (pollfds[i].fd == sockfd && pollfds[i].revents & POLLIN) {
		    	// accept new connection and retrieve new socket file descriptor
				struct sockaddr client_addr;
				socklen_t size = sizeof(client_addr);
				int client_fd;
				if (-1 == (client_fd = accept(sockfd, &client_addr, &size))) {
					//返回值是一个新的套接字描述符，它代表的是和客户端的新的连接，可以把它理解成是一个客户端的socket,这个socket包含的是客户端的ip和port信息 
					perror("Accept");
				}
				
				// display client connection information
				struct sockaddr_in *sockptr = (struct sockaddr_in *)(&client_addr);
				struct in_addr client_address = sockptr->sin_addr;
				

					node = (LinkList*)malloc(sizeof(LinkList));
					
					node->fd = client_fd;
					node->port=ntohs(sockptr->sin_port);
					node->ip= malloc(16);
					strcpy(node->ip,inet_ntoa(client_address));
					
					InsertElem(head,node);
					
				printf("Connection succeeded and client used %s:%hu \n", node->ip, node->port);
				printf("client_fd = %d\n", client_fd);
				int j;
				// store new file descriptor in available slot in the array of struct pollfd set .events to POLLIN
				for (j = 0; j < nfds; j++) {
					if (pollfds[j].fd == -1) {
						pollfds[j].fd = client_fd;
						pollfds[j].events = POLLIN;
                        printf("%d is vide\n",j);
						break;
					}
				}

				// Set .revents of listening socket back to default
				pollfds[i].revents = 0;

			}else if (pollfds[i].fd != sockfd && pollfds[i].revents & POLLIN){
		        // Cleaning memory
                memset(buff, 0, MSG_LEN);
				int rec1;
				int k = 0;
				rec1 = recv(pollfds[i].fd, buff, MSG_LEN, 0);
                // Receiving message
                if ( rec1 == -1) {
					delete_chain(head);
					pollfds[i].events = 0;
					pollfds[i].revents = 0;
                    break;
                }
				if (rec1 == 0){
					printf("client on socket %d has disconnected from server\n", pollfds[i].fd);
					
                    delete_node(head,pollfds[i].fd);
                    close(pollfds[i].fd);
					pollfds[i].fd = -1;
					pollfds[i].events = 0;
					pollfds[i].revents = 0;
                    int j;
                    for(j = 1;j<nfds;j++){
						if(pollfds[j].fd!=-1){
                            printf("%d is active\n",j);
						    k++;
						}   
	 				}
                    fd_active = k;
                    //fd_active--;
					 	printf("still %d mamrche\n",fd_active);
	 					if(fd_active == 0){
							 printf("no more client, server close automatique\n");
							 close(pollfds[0].fd);
							 pollfds[0].events = 0;
							 delete_chain(head); 	

							 return;
						 }
				}
                else{

						if (send(pollfds[i].fd,buff, strlen(buff), 0) <= 0) {
							printf("send echo erreur\n");
							perror("send erreur");
							}

				}


				
            }
	

        }
		///close(sockfd);	
	}

}

int handle_bind(char * port) {
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
		rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		int yes = 1;
		if (-1 == setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		// 第一个元素 描述器序号 第二个元素 SOL_SOCKET： 基本套接口 IPPROTO_IP: IPv4套接口 IPPROTO_IPV6: IPv6套接口 IPPROTO_TCP: TCP套接口
		// 第三个元素 SO_REUSEADDR 允许重用本地地址和端口 第四个元素optval，是一个指向变量的指针 类型：整形，套接口结构， 其他结构类型:linger{}, timeval{ }。
		// 第五个元素： optval 的大小
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

int main(int argc, char *argv[]) {
		// Test argc
	if (argc != 2) {
		printf("Usage: ./server port_number\n");
		exit(EXIT_FAILURE);
	}

	// Create listening socket
	char *port = argv[1];
    
    

	int sfd;
	
	sfd = handle_bind(port);
	if ((listen(sfd, SOMAXCONN)) != 0) {
		perror("listen()\n");
		exit(EXIT_FAILURE);
	}

	echo_server(sfd);
	close(sfd);
	printf("server close\n");
	return EXIT_SUCCESS;
}
