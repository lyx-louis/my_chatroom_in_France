#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include<time.h>
#define SERV_ADDR "127.0.0.1"
#include "msg_struct.h"
#define MSG_LEN 1024
#define NOM_MAX 20
#define IP_MAX 16

typedef struct salon{
    char name[NOM_MAX];
    int number;
    struct salon * next;
    struct salon * prev;
} SalonList;

typedef struct chaineclient{
	int fd;
	int port;
	char * ip;
	time_t t;
    struct chaineclient *prev;
	struct chaineclient *next;
    struct salon * salon;
	char * nickname;
	
} LinkList;
// fonction de salon
SalonList *InsertSalon(SalonList * p,SalonList * elem){
    SalonList * temp = p;
    while(temp->next != NULL){
        temp = temp->next;
    }
    
    elem->next = NULL;
    elem->prev = temp;
    temp->next = elem;
    return p;
}

int  IsSalonUsed(SalonList* head, char* name){
	SalonList * temp = head;
	while(strcmp(temp->name,name)!=0){ // 没找到
		temp = temp->next;
		if(temp == NULL){
			return 0;
		}
	}
	return 1; //找到了
}
SalonList * Find_Salon(SalonList* head, char * name){
	SalonList * temp = head;
	while(strcmp(temp->name,name)!=0){
		temp = temp->next;
		if(temp == NULL){
            printf("[Warning]: no this Salon");
            exit(EXIT_FAILURE);
        }
	}
	return temp;
}
void delete_salon(SalonList *head) 
{
	SalonList *p1;
	while (head)
	{
		p1 = head;
		head = head->next;
		//free(p1->name);
		free(p1);
	}
}

//返回salon节点个数
int ReturnSalonNumber(SalonList * head){
    SalonList * temp = head;
    int i = 0;
    while(temp != NULL){
        i++;
        temp = temp->next;
    }
    return i;
}
// 返回所有节点的名字
char * ReturnSalonName(SalonList * head){	
    SalonList * temp = head->next;
    int length = ReturnSalonNumber(temp);
   // printf("length of list is%d \n",length);
    char * buff;
    buff = (char *)malloc(length * NOM_MAX);
    memset(buff,'\0',sizeof(char)*length*NOM_MAX);
    while(temp != NULL){
        strcat(buff,temp->name);
        strcat(buff,"\n");
        temp = temp->next;
    }
    return buff;
}


void ShowSalon(SalonList* p){
    printf("****************************\n");
   // printf("name: %s \t nombre: %d \n prev: %s\n next : %s\n",p->name,p->number,p->prev->name,p->next->name);
	  printf("name: %s \t nombre: %d\n",p->name,p->number);
	printf("****************************\n");

    printf("\n");
}
int  ReturnUserNumber(SalonList * p){
	return p->number;
}

int DetruireSanon(SalonList * head){
	int userexiste;
	SalonList * temp = head->next;
	while(temp != NULL){
		userexiste = ReturnUserNumber(temp);
		if(userexiste == 0)
			break;
		temp = temp->next;
	}
	if(temp!=NULL){
		if(temp->next == NULL&&temp != NULL){
		SalonList * before = temp->prev;
			before->next = NULL;
			//temp = temp->next;
		}
	else if(temp->next!=NULL&&temp!=NULL){
		SalonList * before = temp->prev;
			SalonList * after = temp ->next;
			//SalonList * dele = temp;
			before->next = after;
			after->prev = before;
			//temp = temp->next;
		}
			//free(temp->name);
			free(temp);
			return 1;
	}
	else{
		return 0;
	}
	
}

void delete1salon(SalonList* p){
	SalonList *temp = p;
	if(temp->next == NULL){
		SalonList * prev = temp->prev;
		prev->next = NULL;
		free(temp);

	}else{
		SalonList * prev = temp->prev;
		SalonList * next = temp->next;
		prev->next = next;
		next->prev = prev;
		free(temp);
	}
}


// fonction de chain
// 删除整个链表
void delete_chain(LinkList *head) 
{
	LinkList *p1;
	while (head)
	{
		p1 = head;
		head = head->next;
		free(p1->ip);
		free(p1->nickname);
		free(p1);
	}
}


// 删除单一节点
LinkList * delete_node(LinkList * head, int fd){
    LinkList * temp = head;
    while(temp->fd!= fd){
        temp = temp->next;
        if(temp == NULL){
            printf("[Warning]: no this node to delete");
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
    free(temp->nickname);
    free(temp);
    return head;
}


// 更新节点
LinkList * Update_Node(LinkList * head, LinkList* nodeadd){
    LinkList * temp = head;
    while(temp->port!= nodeadd->port){
        temp = temp->next;
        if(temp == NULL){
            printf("[Warning]: no this node to update");
            exit(EXIT_FAILURE);
        }
    }
    temp->fd = nodeadd->fd;
    strcpy(temp->ip,nodeadd->ip);
    strcpy(temp->nickname,nodeadd->nickname);
    return head;
}

// 找到节点

LinkList * Find_Node(LinkList* head, char * name){
	LinkList * temp = head;
	while(strcmp(temp->nickname,name)!=0){
		temp = temp->next;
		if(temp == NULL){
            printf("[Warning]: cant find this node");
            exit(EXIT_FAILURE);
        }
	}
	return temp;
}

LinkList * Find_Node_fds(LinkList* head, int fd){
	LinkList * temp = head;
	while(temp->fd!=fd){
		temp = temp->next;
		if(temp == NULL){
            printf("[Warning]: cant find node fds");
            exit(EXIT_FAILURE);
        }
	}
	return temp;
}
// 后插节点
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
// 节点名是否存在
int  IsNameUsed(LinkList* head, char* name){
	LinkList * temp = head;
	while(strcmp(temp->nickname,name)!=0){ // 没找到
		temp = temp->next;
		if(temp == NULL){
			return 0;
		}
	}
	return 1; //找到了
}
//展示节点
void ShowLink(LinkList* p){
    printf("****************************\n");
    printf("fd: %d \t port: %d \n ip: %s\n  nickname : %s\n",p->fd,p->port,p->ip,p->nickname);
    printf("****************************\n");

    printf("\n");
}
// 返回节点个数
int ReturnNumber(LinkList * head){
    LinkList * temp = head;
    int i = 0;
    while(temp != NULL){
        i++;
        temp = temp->next;
    }
    return i;
}
// 返回所有节点的名字
char * ReturnName(LinkList * head){	
    LinkList * temp = head->next;
    int length = ReturnNumber(temp);
   // printf("length of list is%d \n",length);
    char * buff;
    buff = (char *)malloc(length * NOM_MAX);
    memset(buff,'\0',sizeof(char)*length*NOM_MAX);
    while(temp != NULL){
        strcat(buff,temp->nickname);
        strcat(buff,"\n");
        temp = temp->next;
    }
    return buff;
}

void who(struct message msgstruct, char * buff, struct  pollfd pollfds, LinkList * head)
			{
				msgstruct.type = NICKNAME_LIST;
				strcpy(buff,"[Serveur] : Online user are\n");
				char * res = ReturnName(head);
				strcat(buff,res);
				msgstruct.pld_len = strlen(buff);
				if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
				printf("send1 error\n");
					perror("send");
				}
				if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
				printf("send nick list erreur\n");
					perror("send");
				}			
				free(res);	
			}


void whois(struct message msgstruct, char * buff, struct  pollfd pollfds,LinkList *head)
{
    					msgstruct.type = NICKNAME_INFOS;
						LinkList * nodefind;
						char * tofind; 	
						tofind = (char *)malloc(strlen(msgstruct.infos)+1);
						strcpy(tofind,msgstruct.infos);
						nodefind = Find_Node(head,tofind);
						strcpy(buff,"[Server] : User1 connected since");
						struct tm * tm=NULL;
						tm = localtime(&nodefind->t);
						sprintf(buff,"[Server] : %s connected since %d/%d/%d@%d:%d:%d with IP address %s and port number %d\n", 
																nodefind->nickname,
																tm->tm_year+1900,tm->tm_mon,tm->tm_mday,
																tm->tm_hour,tm->tm_min,tm->tm_sec,nodefind->ip,nodefind->port);
						msgstruct.pld_len = strlen(buff);
						if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
						printf("send1 error\n");
						perror("erreur");
						}
						if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
						printf("send nick list erreur\n");
						perror("send erreur");
						}	
						free(tofind);
}

void msgall(struct message msgstruct, char * buff, struct  pollfd pollfds, LinkList * head,LinkList * nodeclient)
{
    					    					int rec2;
						rec2 = recv(pollfds.fd, buff, msgstruct.pld_len, 0);
						if(rec2 == -1){
						perror("Error while receving data\n");
						exit(EXIT_FAILURE);
						}
			
						LinkList * temp;
						temp = head->next;
						int length = ReturnNumber(temp);
						int j;
						for(j =0;j<length;j++){
							if(temp->fd!=pollfds.fd){
								msgstruct.type=BROADCAST_SEND;
								char * userappel = (char *)malloc(MSG_LEN+4);
								strcpy(userappel,"[");
								strcat(userappel,nodeclient->nickname);
								strcat(userappel,"] : ");
								strcat(buff,"\n");
								strcat(userappel,buff);
								msgstruct.pld_len = strlen(userappel);
							
								if (send(temp->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								break;
								}
								if (send(temp->fd,userappel, msgstruct.pld_len, 0) <= 0) {
								printf("send nick list erreur\n");
								break;
								}
								temp= temp->next;
								free(userappel);
							}else {
								temp = temp->next;
								continue;
							}
						
						}
						
						printf("finir broadcast\n");
}

void msg(struct message msgstruct, char * buff, struct  pollfd pollfds, LinkList * head,LinkList * nodeclient)
{
    					int rec2;
						rec2 = recv(pollfds.fd, buff, msgstruct.pld_len, 0);
						if(rec2 == -1){
						perror("Error while receving data\n");
						exit(EXIT_FAILURE);
						}
                        printf("recu : %s, %s\n",msgstruct.infos,buff);
						LinkList *Nodefind;
						int flags = IsNameUsed(head,msgstruct.infos);
						if(flags ==1){
							Nodefind = Find_Node(head,msgstruct.infos);
							strcat(buff,"\n");
							char * userappel = (char *)malloc(NOM_MAX+4);
							strcpy(userappel,"[");
							strcat(userappel,nodeclient->nickname);
							strcat(userappel,"] : ");
							strcat(userappel,buff);
							msgstruct.pld_len = strlen(userappel);
							if (send(Nodefind->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								perror("send erreur");
								}
							if (send(Nodefind->fd,userappel, msgstruct.pld_len, 0) <= 0) {
								printf("send nick list erreur\n");
								perror("send errer");
								}
                                free(userappel);
						}
						else{
							memset(buff,'\0',MSG_LEN);
							strcpy(buff,"[SERVER]: name dont exist,retry\n");
							msgstruct.pld_len = strlen(buff);
							msgstruct.type = SERVER;
							if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								perror("send errer");
								}
							if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
								printf("send nick list erreur\n");
								perror("send erreru");
								}
						}
						printf("send\n");
}
void echo(struct message msgstruct, char * buff,struct  pollfd pollfds)
{
						int rec2;
    					rec2 = recv(pollfds.fd, buff, msgstruct.pld_len, 0);
						if(rec2 == -1){
						perror("Error while receving data\n");
						exit(EXIT_FAILURE);
						}
						if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send1 error\n");
							perror("send erreur");
							}
						if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
							printf("send nick list erreur\n");
							perror("send erreur");
							}
}

void nick(struct message msgstruct, char * buff, struct  pollfd pollfds, LinkList * head,LinkList * nodeclient){
							char namecontrol[NOM_MAX];
							memset(namecontrol,'\0',NOM_MAX);
							strcpy(namecontrol,msgstruct.infos);
							int flags = 0;	
							printf("%s\n",namecontrol);
							int i;
							for(i=0;i<strlen(namecontrol);i++){
								//if((namecontrol[i]<'0') || (namecontrol[i]>'9'&&namecontrol[i]<'a') || (namecontrol[i]>'z'&&namecontrol[i]<'A') || (namecontrol[i]>'Z'))
								if(namecontrol[i]<48)
								{
									flags = 1;
								}
								if(namecontrol[i]>57&&namecontrol[i]<65){
									flags = 1;
								}
								if (namecontrol[i]>90&&namecontrol[i]<97)
								{
									flags = 1;
								}
								if(namecontrol[i]>122){
									flags = 1;
								}
							}
							printf("%d\n",flags);
							if(flags == 0){
										if(IsNameUsed(head,msgstruct.infos)==0&&strlen(msgstruct.nick_sender)==0){
										memset(nodeclient->nickname,'\0',10*sizeof(char));
										strcpy(nodeclient->nickname,msgstruct.infos); // cas no nickname
										printf("name of client on socket %d is %s\n",nodeclient->fd,nodeclient->nickname);
										ShowLink(nodeclient);
										
										strcpy(buff,"[Serveur] : Welcome on the chat,");
										
										strcat(buff,nodeclient->nickname);
										strcat(buff,"\n");

										msgstruct.pld_len = strlen(buff);
										msgstruct.type = SERVER;
										if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
										printf("send1 error\n");
										perror("erreur send");
										}
										if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
										printf("demande nickname error\n");
										perror("erreur send");
										}
										printf("finir send\n");
									}
									else if(IsNameUsed(head,msgstruct.infos)==0&&strlen(msgstruct.nick_sender)!=0){
										
										memset(nodeclient->nickname,'\0',NOM_MAX*sizeof(char));
										strcpy(nodeclient->nickname,msgstruct.infos);
										strcpy(buff,"[Serveur] : name of ");
										strcat(buff,msgstruct.nick_sender);
										strcat(buff," was changed to ");
										strcat(buff,nodeclient->nickname);
										strcat(buff,"\n");
										//sprintf(buff,"[Serveur] : name of %s was changed to %s\n",msgstruct.nick_sender,nodeclient->nickname);
										msgstruct.pld_len = strlen(buff);
										msgstruct.type = SERVER;
										ShowLink(nodeclient);
										if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
										printf("send1 error\n");
										perror("erreur send");
										}
										if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
										printf("demande nickname error\n");
										perror("erreur send");
										}
										printf("finir send\n");
									}
									else if(IsNameUsed(head,msgstruct.infos)==1){
										msgstruct.type = ERREUR_NICKNAME_NEW; // ERREUR
										strcpy(buff,"[Serveur] : Sorry, the name is already existe\n");
										msgstruct.pld_len = strlen(buff);
										if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
										printf("send1 error\n");
										perror("erreur send");
										}
										if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
										printf("demande nickname error\n");
										perror("erreur send");
										}				
									}
							}
							else{
								msgstruct.type = ERREUR_NICKNAME_NEW; // ERREUR
										strcpy(buff,"[Serveur] : Sorry, name should only be number and caractere\n");
										msgstruct.pld_len = strlen(buff);
										if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
										printf("send1 error\n");
										perror("erreur send");
										}
										if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
										printf("demande nickname error\n");
										perror("erreur send");
										}
							}
							
}
void talksalon(struct message msgstruct, char * buff, struct  pollfd pollfds, LinkList * head,LinkList * nodeclient)
{
						//int num = ReturnNumber(nodeclient->salon);
						int rec2;
						rec2 = recv(pollfds.fd, buff, msgstruct.pld_len, 0);
						if(rec2 == -1){
						perror("Error while receving data\n");
						exit(EXIT_FAILURE);
						}
						
						LinkList * temp = head;
						while(temp!=NULL){
							if(temp->salon == nodeclient->salon&& temp!=nodeclient){
								char * userappel = (char *)malloc(MSG_LEN+4);
								strcpy(userappel,"[");
								strcat(userappel,nodeclient->nickname);
								strcat(userappel,"] : ");
								strcat(userappel,buff);
								msgstruct.pld_len = strlen(userappel);
								if (send(temp->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								break;
								}
								if (send(temp->fd,userappel, msgstruct.pld_len, 0) <= 0) {
								printf("send nick list erreur\n");
								break;
								}
								temp= temp->next;
								free(userappel);
							}
							else{
								temp = temp->next;
							}
						}
}
void createsalon(struct message msgstruct, char * buff, struct  pollfd pollfds,LinkList * nodeclient,SalonList *salonhead,LinkList *head)
{
	                    	char namecontrol[NOM_MAX];
							memset(namecontrol,'\0',NOM_MAX);
							strcpy(namecontrol,msgstruct.infos);
							int flags = 0;	
							int i;
							for(i=0;i<strlen(namecontrol);i++){
								//if((namecontrol[i]<'0') || (namecontrol[i]>'9'&&namecontrol[i]<'a') || (namecontrol[i]>'z'&&namecontrol[i]<'A') || (namecontrol[i]>'Z'))
								if(namecontrol[i]<48)
								{
									flags = 1;
								}
								if(namecontrol[i]>57&&namecontrol[i]<65){
									flags = 1;
								}
								if (namecontrol[i]>90&&namecontrol[i]<97)
								{
									flags = 1;
								}
								if(namecontrol[i]>122){
									flags = 1;
								}
							}
						if(flags == 0){
							int k = IsSalonUsed(salonhead,msgstruct.infos);
                        if(k == 0){
							if(nodeclient->salon != NULL){
										LinkList * temp = head;
								while(temp!=NULL){
								if(temp->salon == nodeclient->salon&& temp!=nodeclient){
									
									strcpy(buff,"[");
									strcat(buff,nodeclient->nickname);
									strcat(buff,"] has quit the salon ");
									strcat(buff,nodeclient->salon->name);
									strcat(buff,"\n");
									msgstruct.pld_len = strlen(buff);
									printf("buff is %s\n",buff);
									if (send(temp->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
									printf("send1 error\n");
									perror("send erreur");
									}
									if (send(temp->fd,buff, msgstruct.pld_len, 0) <= 0) {
									printf("send nick list erreur\n");
									perror("send erreur");
									}
									temp= temp->next;
								}
								else{
									temp = temp->next;
								}
								}

								int detruire;	
								nodeclient->salon->number --;
								char * salonname;
								salonname = (char *)malloc(NOM_MAX);
								strcpy(salonname,nodeclient->salon->name);
								detruire = DetruireSanon(salonhead);
								if(detruire == 1){ // 房间没人了 输出
									msgstruct.type = SERVER;
									strcpy(buff,"You were the last user in this channel, ");
									strcat(buff,salonname);
									strcat(buff," has been destroyed\n");
									msgstruct.pld_len = strlen(buff);
									if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
									printf("send1 error\n");
									perror("send erreur");
									}
									if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
									printf("demande nickname error\n");
									perror("send erreur");
									}
								}
								//ShowSalon(nodeclient->salon);
								free(salonname);
							}
                            SalonList * creatsalon;
                            creatsalon = (SalonList*)malloc(sizeof(SalonList));
                            creatsalon->prev = NULL;
                            creatsalon->next = NULL;
                            memset(creatsalon->name,'\0',NOM_MAX*sizeof(char));
                            strcpy(creatsalon->name,msgstruct.infos);
                            creatsalon->number = 1;
                            InsertSalon(salonhead,creatsalon);
                            nodeclient->salon = creatsalon;
                            ShowSalon(creatsalon);
                        }
						else{
							msgstruct.type = ERREUR_CREATE_SALON; // ERREUR
							strcpy(buff,"[Serveur] : Sorry, the name of shanon is already existe\n");
							msgstruct.pld_len = strlen(buff);
							if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send1 error\n");
							perror("send erreur");
							}
							if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
							printf("demande nickname error\n");
							perror("send erreur");
							}	
						}
					}
					else{
							msgstruct.type = ERREUR_CREATE_SALON; // ERREUR
							strcpy(buff,"[Serveur] : Sorry, the name of shanon should only be number and charactere\n");
							msgstruct.pld_len = strlen(buff);
							if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send1 error\n");
							perror("send erreur");
							}
							if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
							printf("demande nickname error\n");
							perror("send erreur");
							}	
					}
						
                    
				
}

void listsalon(struct message msgstruct, char * buff, struct  pollfd pollfds,SalonList * salonhead){
						msgstruct.type = NICKNAME_LIST;
						strcpy(buff,"[Serveur] : Salon existe \n");
						char * res = ReturnSalonName(salonhead);
									strcat(buff,res);
								msgstruct.pld_len = strlen(buff);
								if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
									printf("send1 error\n");
									perror("send erreur");
								}
								if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
									printf("send nick list erreur\n");
									perror("send erruer");
								}			
								free(res);
					
					}
						

void joinsalon(struct message msgstruct, char * buff, struct  pollfd pollfds, LinkList * head,LinkList * nodeclient,SalonList * salonhead)
{
	//printf("recu!%s,%d\n",msgstruct.infos,strlen(msgstruct.infos));
						int k = IsSalonUsed(salonhead,msgstruct.infos);
						if(k==0){
							msgstruct.type = ERREUR_FIND_SALON; // ERREUR
							strcpy(buff,"[Serveur] : Sorry, can't find this salon\n");
							msgstruct.pld_len = strlen(buff);
							if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send1 error\n");
							perror("send erreur");
							}
							if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
							printf("demande nickname error\n");
							perror("send erreur");
							}
						}else{
							if(nodeclient->salon != NULL){ // 从房间到另一个房间
								LinkList * temp = head;
								while(temp!=NULL){
								if(temp->salon == nodeclient->salon&& temp!=nodeclient){
									
									strcpy(buff,"[");
									strcat(buff,nodeclient->nickname);
									strcat(buff,"] has quit the salon ");
									strcat(buff,nodeclient->salon->name);
									strcat(buff,"\n");
									msgstruct.pld_len = strlen(buff);
									printf("buff is %s\n",buff);
									if (send(temp->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
									printf("send1 error\n");
									perror("send erreur");
									}
									if (send(temp->fd,buff, msgstruct.pld_len, 0) <= 0) {
									printf("send nick list erreur\n");
									perror("send erreur");
									}
									temp= temp->next;
								}
								else{
									temp = temp->next;
								}
								}

								int detruire;	
								nodeclient->salon->number --;
								char * salonname;
								salonname = (char *)malloc(NOM_MAX);
								strcpy(salonname,nodeclient->salon->name);
								detruire = DetruireSanon(salonhead);
								if(detruire == 1){ // 房间没人了 输出
									msgstruct.type = SERVER;
									strcpy(buff,"You were the last user in this channel, ");
									strcat(buff,salonname);
									strcat(buff," has been destroyed\n");
									msgstruct.pld_len = strlen(buff);
									if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
									printf("send1 error\n");
									perror("send erreur");
									}
									if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
									printf("demande nickname error\n");
									perror("send erreur");
									}
								}
								//ShowSalon(nodeclient->salon);
								free(salonname);
							}// 加入房间

							SalonList * find;
							printf("1\n");
							find = Find_Salon(salonhead,msgstruct.infos);
							find->number++;
							nodeclient->salon = find;
							LinkList * temp = head;
							while(temp!=NULL){
								if(temp->salon == nodeclient->salon&& temp!=nodeclient){
									
									strcpy(buff,"[");
									strcat(buff,nodeclient->nickname);
									strcat(buff,"] has joined the salon ");
									strcat(buff,find->name);
									strcat(buff,"\n");
									msgstruct.pld_len = strlen(buff);
									printf("buff is %s\n",buff);
									if (send(temp->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
									printf("send1 error\n");
									perror("send erreur");
									}
									if (send(temp->fd,buff, msgstruct.pld_len, 0) <= 0) {
									printf("send nick list erreur\n");
									perror("send erreur");
									}
									temp= temp->next;
								}
								else{
									temp = temp->next;
								}
							}
							
							
							msgstruct.type = SERVER;
							strcpy(buff,"[Serveur] : You have joined the salon ");
							strcat(buff,msgstruct.infos);
							strcat(buff,"\n");
							msgstruct.pld_len = strlen(buff);
							ShowSalon(find);
							if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send1 error\n");
							perror("send erreur");
							}
							if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
							printf("demande nickname error\n");
							perror("send erreur");
							}
						}
}

void quitsalon(struct message msgstruct, char * buff, struct  pollfd pollfds, LinkList * head,LinkList * nodeclient,SalonList * salonhead){
							//printf("recu!%s,%d\n",msgstruct.infos,strlen(msgstruct.infos));
						int k = IsSalonUsed(salonhead,msgstruct.infos);
						if(k==0){
							msgstruct.type = ERREUR_FIND_SALON; // ERREUR
							strcpy(buff,"[Serveur] : Sorry, can't find this salon\n");
							msgstruct.pld_len = strlen(buff);
							if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send1 error\n");
							perror("send erreur");
							}
							if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
							printf("demande nickname error\n");
							perror("send erreur");
							}
						}else if(nodeclient->salon == NULL){
							msgstruct.type = ERREUR_FIND_SALON; // ERREUR
							strcpy(buff,"[Serveur] : Sorry, you dont have a group to quit\n");
							msgstruct.pld_len = strlen(buff);
							if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
							printf("send1 error\n");
							perror("send erreur");
							}
							if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
							printf("demande nickname error\n");
							perror("send erreur");
							}
						}

						else{
							int detruire;
							SalonList * find;
							find = Find_Salon(salonhead,msgstruct.infos);
							find->number--;
							LinkList * temp = head;
							while(temp!=NULL){
							if(temp->salon == nodeclient->salon&& temp!=nodeclient){
								
								strcpy(buff,"[");
								strcat(buff,nodeclient->nickname);
								strcat(buff,"] has quit the salon ");
								strcat(buff,find->name);
								strcat(buff,"\n");
								msgstruct.pld_len = strlen(buff);
								printf("buff is %s\n",buff);
								if (send(temp->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								perror("send erreur");
								}
								if (send(temp->fd,buff, msgstruct.pld_len, 0) <= 0) {
								printf("send nick list erreur\n");
								perror("send erreur");
								}
								temp= temp->next;
							}
							else{
								temp = temp->next;
							}
							}


							detruire = DetruireSanon(salonhead);
							if(detruire == 0){
								nodeclient->salon = NULL;
								msgstruct.type = SERVER;
								strcpy(buff,"[Serveur] : You have quit the salon ");
								strcat(buff,msgstruct.infos);
								strcat(buff,"\n");
								msgstruct.pld_len = strlen(buff);
								ShowSalon(find);
								if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								perror("send erreur");
								}
								if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
								printf("demande nickname error\n");
								perror("send erreur");
								}
							}
							else if(detruire ==1){
								nodeclient->salon = NULL;
								msgstruct.type = SERVER;
								strcpy(buff,"You were the last user in this channel, ");
								strcat(buff,msgstruct.infos);
								strcat(buff," has been destroyed\n");
								msgstruct.pld_len = strlen(buff);
								//ShowSalon(find);
								if (send(pollfds.fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								perror("send erreur");
								}
								if (send(pollfds.fd,buff, msgstruct.pld_len, 0) <= 0) {
								printf("demande nickname error\n");
								perror("send erreur");
								}
							}
						}
}

void echo_server(int sockfd, char * port) {
	int nfds = 10; //max client
     struct pollfd pollfds[nfds];
     // Init first slot with listening socket
	 pollfds[0].fd = sockfd;
	 pollfds[0].events = POLLIN; // 普通或优先带数据可读
	 pollfds[0].revents = 0;
	 // Init remaining slot to default values
	 int i;
	 for (i = 1; i < nfds; i++) {
	 	pollfds[i].fd = -1;
	 	pollfds[i].events = 0;
	 	pollfds[i].revents = 0;
	 }

	int rec1 = -1;
	struct message msgstruct;
	char buff[MSG_LEN];
	
	LinkList * head;
	head = (LinkList*)malloc(sizeof(LinkList));
	//memset(&head, 0, sizeof(struct chaineclient));
	head->port = atoi(port);
	
	head->fd = sockfd;
	head->prev = NULL;
	head->next = NULL;
	head->ip = (char *)malloc(IP_MAX);
	memset(head->ip,'\0',10*sizeof(char));
	head->nickname = (char  *)malloc(NOM_MAX);
	head->t = time(NULL);
	strcpy(head->nickname,"server");
	ShowLink(head);
	
    SalonList * salonhead;
    salonhead = (SalonList*)malloc(sizeof(SalonList));
    salonhead->prev = NULL;
    salonhead->next = NULL;
    memset(salonhead->name,'\0',NOM_MAX*sizeof(char));
    salonhead->number = 1;
    head->salon = salonhead; // v
    
    int fd_active = 0;
	while (1) {
	    int n_active = 0;
	 if (-1 == (n_active = poll(pollfds, nfds, -1))) {
	 	perror("Poll");
	 }
	 int i;
	 for(i = 0;i<nfds;i++){
		 if (pollfds[i].fd == sockfd && pollfds[i].revents & POLLIN) {
		 		struct sockaddr client_addr;
				socklen_t size = sizeof(client_addr);
				int client_fd;
				memset(&msgstruct, 0, sizeof(struct message));
				memset(buff, 0, MSG_LEN);
				if (-1 == (client_fd = accept(sockfd, &client_addr, &size))) {
					perror("Accept");
				}
				
				struct sockaddr_in *sockptr = (struct sockaddr_in *)(&client_addr);
				struct in_addr client_address = sockptr->sin_addr;
				
				LinkList * node;
				node = (LinkList*)malloc(sizeof(LinkList));
				//memset(node,'\0',sizeof(node));
				node->fd = client_fd;
				node->port = ntohs(sockptr->sin_port);
				node->t = time(NULL);
				node->ip = (char *)malloc(IP_MAX);
				node->salon = NULL;
				strcpy(node->ip,inet_ntoa(client_address));
				node->nickname = (char *)malloc(NOM_MAX);
				memset(node->nickname,'\0',10*sizeof(char));
				InsertElem(head,node);
				
				ShowLink(node);
    
				printf("\nConnection succeeded and client used %s:%hu \n", head->next->ip, head->next->port);
				printf("client_fd = %d\n\n", node->fd);
				// store new file descriptor in available slot in the array of struct pollfd set .events to POLLIN
				int j;
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

				if(strlen(node->nickname)==0){
					strcpy(buff,"[Server] : please login with /nick <your pseudo>\n");
					msgstruct.pld_len = strlen(buff);
					msgstruct.type = SERVER;
					if (send(client_fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
					printf("send1 error\n");
					break;
					}
					if (send(client_fd, buff, msgstruct.pld_len, 0) <= 0) {
					printf("demande nickname error\n");
					break;
					}
					
				}
		 }
		 else if(pollfds[i].fd != sockfd && pollfds[i].revents & POLLIN){
			LinkList *nodeclient;
			memset(&msgstruct, 0, sizeof(struct message));
			memset(buff, 0, MSG_LEN);
			nodeclient = Find_Node_fds(head,pollfds[i].fd);			
            int k = 0;
			 rec1 = recv(pollfds[i].fd,&msgstruct, sizeof(struct message), 0);
				if(rec1 == -1){
					perror("Error while receiving structure\n");
					exit(EXIT_FAILURE);
				}
				if(rec1 == 0){
					printf("client on socket %d has disconnected from server\n", pollfds[i].fd);
					if(nodeclient->salon!=NULL){
						nodeclient->salon->number --;
						ShowSalon(nodeclient->salon);
						if(nodeclient->salon->number == 0){
							delete1salon(nodeclient->salon); 
						}
						
						LinkList * temp = head;
							while(temp!=NULL){
							if(temp->salon == nodeclient->salon&& temp!=nodeclient){
								sprintf(buff,"[%s] has quit the salon %s\n",nodeclient->nickname,nodeclient->salon->name);
								msgstruct.pld_len = strlen(buff);
								if (send(temp->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
								printf("send1 error\n");
								break;
								}
								if (send(temp->fd,buff, msgstruct.pld_len, 0) <= 0) {
								printf("send nick list erreur\n");
								break;
								}
								
							}
								temp= temp->next;
							}
							nodeclient->salon = NULL;
					}
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
					 	printf("still %d mamrche\n",fd_active);
	 					if(fd_active == 0){
							 printf("no more client, server close automatique\n");
							 close(pollfds[0].fd);
							 pollfds[0].events = 0;
							 delete_chain(head); 	
							 delete_salon(salonhead);
							 return;
						 }
				}
				else{            
					if(msgstruct.type==NICKNAME_NEW){
						nick(msgstruct,buff,pollfds[i],head,nodeclient);	
					}
					if(msgstruct.type==NICKNAME_LIST){
					who(msgstruct,buff,pollfds[i],head);	
					}
					if(msgstruct.type==NICKNAME_INFOS){
						whois(msgstruct,buff,pollfds[i],head);
					}
					if(msgstruct.type == BROADCAST_SEND){
						msgall(msgstruct,buff,pollfds[i],head,nodeclient);					
					}
					if(msgstruct.type == UNICAST_SEND){
						msg(msgstruct,buff,pollfds[i],head,nodeclient);
					}
					if(msgstruct.type == ECHO_SEND){
						echo(msgstruct,buff,pollfds[i]);
					}
                    if(msgstruct.type == MULTICAST_CREATE){
						createsalon(msgstruct,buff,pollfds[i],nodeclient,salonhead,head);
					}
					if(msgstruct.type == MULTICAST_LIST) {
						listsalon(msgstruct,buff,pollfds[i],salonhead);
					}
					if(msgstruct.type == MULTICAST_JOIN){
						joinsalon(msgstruct,buff,pollfds[i],head,nodeclient,salonhead);
					}
					if(msgstruct.type == MULTICAST_QUIT){					
						quitsalon(msgstruct,buff,pollfds[i],head,nodeclient,salonhead);
					}
					if(msgstruct.type == MULTICAST_SEND){
						talksalon(msgstruct,buff,pollfds[i],head,nodeclient);				
					}
				}// else		 
		 } // else if*/
	 } // for
	} // while
} // fonction
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
		int yes = 1;

		if (sfd == -1) {
			continue;
		}
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

int main(int argc, char* argv[]) {
		if (argc != 2) {
		printf("Usage: ./server port_number\n");
		exit(EXIT_FAILURE);
	}
	char *port = argv[1];

	int sfd;

	sfd = handle_bind(port);
	if ((listen(sfd, SOMAXCONN)) != 0) {
		perror("listen()\n");
		exit(EXIT_FAILURE);
	}
	
	echo_server(sfd, port);
	close(sfd);
	return EXIT_SUCCESS;
}

