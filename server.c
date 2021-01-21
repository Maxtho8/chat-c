#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <string.h>
#include <sys/types.h>   
#include <unistd.h>

       /* Voir NOTES */
#define SERVADDR "127.0.0.1"        // Définition de l'adresse IP d'écoute
#define SERVPORT "48963"                // Définition du port d'écoute, 
                                    // si 0 port choisi dynamiquement
#define LISTENLEN 2                 // Taille du tampon de demande de connexion
#define MAXBUFFERLEN 1024
#define MAXHOSTLEN 64
#define MAXPORTLEN 6

int main(){
	int ecode;                       // Code retour des fonctions
	char serverAddr[MAXHOSTLEN];     // Adresse du serveur
	char serverPort[MAXPORTLEN];     // Port du server
	int descSockRDV;                 // Descripteur de socket de rendez-vous
	int descSockCOM;                 // Descripteur de socket de communication
	struct addrinfo hints;           // Contrôle la fonction getaddrinfo
	struct addrinfo *res;            // Contient le résultat de la fonction getaddrinfo
	struct sockaddr_storage myinfo;  // Informations sur la connexion de RDV
	struct sockaddr_storage from;    // Informations sur le client connecté
	socklen_t len;                   // Variable utilisée pour stocker les 
				                        // longueurs des structures de socket
	char buffer[MAXBUFFERLEN];       // Tampon de communication entre le client et le serveur
    
	// Publication de la socket au niveau du système
	// Assignation d'une adresse IP et un numéro de port
	// Mise à zéro de hints
	memset(&hints, 0, sizeof(hints));
	// Initailisation de hints
    hints.ai_family = AF_INET;                                                                                              // Set IP family to IPv4
    hints.ai_socktype = SOCK_STREAM;                                                                                        // Set socket type to stream
    hints.ai_protocol = 0;      // protocole automatique

	// Récupération des informations du serveur
	ecode = getaddrinfo(NULL, SERVPORT, &hints, &res);
	if (ecode) {
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
		exit(1);
	}

	//Création de la socket IPv4/TCP
	descSockRDV = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (descSockRDV == -1) {
		perror("Erreur creation socket");
		exit(4);
	}

	// Publication de la socket
	ecode = bind(descSockRDV, res->ai_addr, res->ai_addrlen);
	if (ecode == -1) {
		perror("Erreur liaison de la socket de RDV");
		exit(3);
	}
	// Nous n'avons plus besoin de cette liste chainée addrinfo

	printf("L'adresse d'ecoute est: %s\n", SERVADDR);
	printf("Le port d'ecoute est: %s\n", SERVPORT);
	// Definition de la taille du tampon contenant les demandes de connexion
	ecode = listen(descSockRDV, LISTENLEN);
	if (ecode == -1) {
		perror("Erreur initialisation buffer d'écoute");
		exit(5);
	}

	len = sizeof(struct sockaddr_storage);
	// Attente connexion du client
	// Lorsque demande de connexion, creation d'une socket de communication avec le client
	int socket[2];
	int tubeun[2];
	int tubedeux[2];




	int pid  = 0;
	pid = fork();
	if(pid != 0){
		close(tubeun[1]);
	}
	if(pid == 0){
		socket[0] = accept(descSockRDV, (struct sockaddr *) &from, &len);
		if (socket[0] == -1){
			perror("Erreur accept\n");
			exit(6);
		}
		strcpy(buffer, "Waiting another user...\n");
		write(socket[0], buffer, strlen(buffer));
		close(tubeun[0]);
		memset(buffer, 0x00, 1024);
		while(1){
			// Echange de données avec le client connecté
			ecode = read(socket[0], buffer, 1024);
			printf("Recu client 0 : %s\n",buffer);
			write(tubeun[1],buffer,strlen(buffer));
			sleep(1);
			memset(buffer, 0x00, 1024);
		}
    }
	

	int piddeux = 0;
	piddeux = fork();
	if(piddeux!= 0){
		close(tubedeux[1]);
	}
	if(piddeux == 0){
		socket[1] = accept(descSockRDV, (struct sockaddr *) &from, &len);
		if (socket[1] == -1){
			perror("Erreur accept\n");
			exit(6);
		}
		strcpy(buffer, "Welcome to the chat\n");
		write(socket[1], buffer, strlen(buffer));
		close(tubedeux[0]);
		memset(buffer, 0x00, 1024);
		while(1){
			// Echange de données avec le client connecté
			ecode = read(socket[1], buffer, 1024);
			printf("Recu client 1 : %s\n",buffer);
			write(tubedeux[1],buffer,strlen(buffer)+1);
			sleep(1);
			memset(buffer, 0x00, 1024);
		}
    }
		


	char tampon[1024];
	printf("Je commence à lire\n");   
    while(1){
		
		memset(tampon, 0x00, 1024);
		read(tubeun[0],tampon,sizeof(tampon));
		
		printf("Lu par le fils 1 : %s\n", tampon);
		
		memset(tampon, 0x00, 1024);
    	read(tubedeux[0],tampon,sizeof(tampon));
    
		printf("Lu par le fils 2 : %s\n", tampon);
		
		sleep(2);
			
		
    }

	close(descSockCOM);
	close(descSockRDV);
}



