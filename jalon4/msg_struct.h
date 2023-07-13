#define NICK_LEN 128
#define INFOS_LEN 128

enum msg_type { 
	NICKNAME_NEW,
	NICKNAME_LIST,
	NICKNAME_INFOS,
	ECHO_SEND,
	UNICAST_SEND, 
	BROADCAST_SEND,
	MULTICAST_CREATE,
	MULTICAST_LIST,
	MULTICAST_JOIN,
	MULTICAST_SEND,
	MULTICAST_QUIT,
	FILE_REQUEST,
	FILE_ACCEPT,
	FILE_REJECT,
	FILE_SEND,
	FILE_ACK,
	ERREUR_NICKNAME_NEW,
	SERVER,
	ERREUR,
	ERREUR_FIND_SALON,
	ERREUR_CREATE_SALON
};

struct message {
	int pld_len; //taille des données utiles à envoyer (généralement, la taille du message à envoyer). S’il n’y a pas de données utiles 
	//(par exemple la requête en question n’a pas de données de message ou de fichier), pld_len doit valoir 0.
	char nick_sender[NICK_LEN];
	enum msg_type type; 
	// type de message (cf la liste dans enum msg_type). Ce gens va être utilisé pour différencier les actions demandées par l’utilisateur.
	char infos[INFOS_LEN];
	// contenir des informations à envoyer. Ces informations dépendent du champ type.
};

/*static char* msg_type_str[] = {
	"NICKNAME_NEW",
	"NICKNAME_LIST",
	"NICKNAME_INFOS",
	"ECHO_SEND",
	"UNICAST_SEND", 
	"BROADCAST_SEND",
	"MULTICAST_CREATE",
	"MULTICAST_LIST",
	"MULTICAST_JOIN",
	"MULTICAST_SEND",
	"MULTICAST_QUIT",
	"FILE_REQUEST",
	"FILE_ACCEPT",
	"FILE_REJECT",
	"FILE_SEND",
	"FILE_ACK",
	"ERREUR_NICKNAME_NEW",
	"SERVER"
};*/