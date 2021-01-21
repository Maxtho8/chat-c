#include  <stdio.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <string.h>
#include  <stdlib.h>
#include  <stdbool.h>

#define MAXHOSTLEN 64
#define MAXPORTLEN 6
#define MAXBUFFERLEN 1024

int main(int argc, char* argv[]){
	int descSock;                  // Descripteur de la socket
	int ecode;                     // Retour des fonctions
	struct addrinfo *res,*resPtr;  // Résultat de la fonction getaddrinfo
	struct addrinfo hints;
	char serverName[MAXHOSTLEN];   // Nom de la machine serveur
	char serverPort[MAXPORTLEN];   // Numéro de port
	char buffer[MAXBUFFERLEN];     // Buffer stockant les messages entre 
                                  // le client et le serveur
	bool isConnected = false;      // booléen indiquant que l'on est bien connecté
	char *pseudo;
	int nb;
	size_t line_buf_size = 0;
	printf("Entrez le pseudo : ");
    nb = getline(&pseudo, &line_buf_size, stdin);
    // Initailisation de hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;  // TCP
	hints.ai_family = AF_UNSPEC;      // les adresses IPv4 et IPv6 seront présentées par 
				                          // la fonction getaddrinfo

	//Récupération des informations sur le serveur
	ecode = getaddrinfo("127.0.0.1","48963",&hints,&res);
	if (ecode){
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
		exit(1);
	}

	resPtr = res;

	while(!isConnected && resPtr!=NULL){

		//Création de la socket IPv4/TCP
		descSock = socket(resPtr->ai_family, resPtr->ai_socktype, resPtr->ai_protocol);
		if (descSock == -1) {
			perror("Erreur creation socket");
			exit(2);
		}
		printf("FAMILY : %d \n",resPtr->ai_family);
  
  		//Connexion au serveur
		ecode = connect(descSock, resPtr->ai_addr, resPtr->ai_addrlen);
		if (ecode == -1) {
			resPtr = resPtr->ai_next;    		
			close(descSock);	
		}
		// On a pu se connecté
		else isConnected = true;
	}
	freeaddrinfo(res);
	if (!isConnected){
		perror("Connexion impossible");
		exit(2);
	}
	//Echange de donneés avec le serveur
	ecode = read(descSock, buffer, MAXBUFFERLEN);
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0';
	printf("SERVER: %s.\n",buffer);
	
	
	char *msg;
	size_t test = 0;
	memset(&msg, 0, sizeof(msg));
	char testt[20];
	while(1){

			//Obtenir message
    		nb = getline(&msg, &test, stdin);
			write(descSock, msg, 20);
			strcpy(testt, "test mon reuf\n");
			write(descSock, testt, strlen(testt));
			printf("Envoi de ; %s\n",msg);
			memset(msg, 0, sizeof(msg));
			
		

	}
	//Fermeture de la socket
	//close(descSock);

}

