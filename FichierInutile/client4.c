#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>
#include <dirent.h>

#define longueurMessage 10000

// Connecté ou pas!
int status = 0;

void connection(int socketClient)
{
    int ecrits;

    // Reception des données du serveur
    int lus;
    char messageRecu[longueurMessage];
    int socket = (long)socketClient; 
    
    // le message de la couche application ! 
    char messageEnvoi[longueurMessage];
    
    // Envoie un message au serveur et gestion des erreurs
    // sprintf(messageEnvoi, "Holla");
    int i = 0;
    while(status == 0)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        if( i < 1)
        {
            strcpy(messageEnvoi, "tentative de connexion");
        } else {            
            fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
            messageEnvoi[strlen(messageEnvoi) - 1]= 0;
        }
        ecrits = write(socket, messageEnvoi,strlen(messageEnvoi));
        switch(ecrits)
        {
            case -1: 
                perror("write");
                close(socket);
                exit(-3);
            case 0:
                fprintf(stderr, "\n[-]La socket a été fermée par le serveur !\n");
                close(socket);
                exit(-5);
            default:
                if( i < 1)
                {
                    printf("Demande de connexion ...\n");
                } else {            
                    printf("Pseudo %s envoyé avec succés ☻ (%d octets)\n",messageEnvoi,ecrits);
                }
        }

        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        lus = read(socket, messageRecu, longueurMessage*sizeof(char)); 
        switch(lus)
        {
            case -1: 
                perror("read");
                close(socket);
                exit(-4);
            case 0:
                fprintf(stderr, "\n[-]La socket a été fermée par le serveur !\n");
                close(socket);
                exit(-5);
            default:
                if(status == 0)
                {
                    if(strcmp(messageRecu, "valide") == 0)
                    {
                        status = 1;
                        printf("\nLe pseudo choisit est bien valide ☻ \n");
                        break;
                    }else if(strcmp(messageRecu, "invalide") == 0)
                    {
                        printf("\nLe pseudo choisit exist déja !!\n");
                        printf("Choisissez un autre pseudo\n");
                        printf("→ ");   
                    }
                    else {
                        i++;
                        puts(messageRecu);
                        printf("→ ");
                    }
                } 
        }
    }    
}

/**
 * @brief   
 * Lister les fichiers disponible dans un dossier
 * et retourner le nombre
 * @param dossier 
 * @return int fichier nombre de fichiers disponible
 */
int listeFichierDansDos(char * dossier, char ** fichiers)
{
    struct dirent *dir;
    int nbFichiers = 0;
    // opendir() renvoie un pointeur de type DIR. 
    DIR *d = opendir(dossier); 
    if (d)
    {
        printf("\n Listes des fichiers possibles d'envoyer \n");
        while ((dir = readdir(d)) != NULL)
        {
            fichiers[nbFichiers] = malloc(256*sizeof(char)); 
            fichiers[nbFichiers] = dir->d_name;
            printf("-> %s\n",dir->d_name);
            nbFichiers++;
        }
        closedir(d);
    }
    return nbFichiers;
}


/**
 * @brief 
 * Retourne la taille d'un fichier
 * @param filename fichier duquel on veut connaître la taille
 * @return Un entier qui est la taille du fichier
 */
int tailleFile(char * filename){
    int handle;
    long taille;
    handle = open(filename, O_RDONLY);
    taille = lseek(handle, 0, SEEK_END );
    close(handle);
    return taille;
}


 /**
  * @brief 
  * La fonction envoie un fichier
  * @param fp le fichier a envoyer au servuer
  * @param sockfd socket de client
  */
void send_file(FILE *fp,char*filename,int sockfd){
  int n;
  int taille = tailleFile(filename);
  printf("la taille de fichier est %d \n", taille);
  char *data= malloc(taille*sizeof(char));
 
  while(fgets(data, taille, fp) != NULL) {
    if (send(sockfd, data, taille, 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, taille);
  }
}

void envoieFichier(int socket, char * messageEnvoi)
{
    char ** fichiers = malloc(256*sizeof(char));
    FILE *fp;
    char *nomFichier = malloc(longueurMessage*sizeof(char));
    char *dossier = malloc(longueurMessage*sizeof(char));
    int i , nombreFichiers, fichiersTrouver;

    if (write(socket, "file",strlen("file")) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }


    memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
    strcpy(dossier, "Transferer");   
    nombreFichiers = listeFichierDansDos(dossier, fichiers);
    fichiersTrouver = 0;
    while(fichiersTrouver == 0){
        puts("\n☼☼☼ Choisissez un fichier ☼☼☼");
        printf("→ ");
        fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
        messageEnvoi[strlen(messageEnvoi) - 1]=0;
        for(i=0;i<nombreFichiers;i++)
        {     
            if(strcmp(messageEnvoi,fichiers[i]) == 0)
            {
                printf("Le fichier existe bien !\n");
                fichiersTrouver = 1;
                strcpy(nomFichier,messageEnvoi);
                strcat(dossier, "/");
                strcat(dossier,nomFichier);
                fp = fopen(dossier, "r");
                if (fp == NULL) {
                    perror("[-]Error a la lecture du fichier.");
                    exit(1);
                }
                printf("le fichier a été lu correctement \n");
                
                send_file(fp, dossier, socket);
                printf("[+]Fichier a été bien envoyer.\n");
            }
        }
        if(fichiersTrouver == 0)
        {
            printf("! Ce fichier n'existe pas !\n");
        }
    }
}

//fonction qui va servir au thread pour l'envoi de messages
void * Envoyer(void * socketClient)
{
    int ecrits;
    int socket = (long)socketClient;
    // le message de la couche application ! 
    char messageEnvoi[longueurMessage];
    // Envoie un message au serveur et gestion des erreurs
    printf("► Envoyer /help pour voir la liste des commandes possible ◄\n► Envoyer file pour envoyer un fichier ◄\n");
    while(1)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        puts("\n☼☼☼ Envoyer Un Message ☼☼☼");
        printf("→ ");
        fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
        messageEnvoi[strlen(messageEnvoi) - 1]=0;
        if(strcmp(messageEnvoi, "file") == 0)
        {
            envoieFichier(socket, messageEnvoi);
        } else 
        {    
            ecrits = write(socket, messageEnvoi,strlen(messageEnvoi));
            switch(ecrits)
            {
                case -1: 
                    perror("write");
                    close(socket);
                    exit(-3);
                case 0:
                    fprintf(stderr, "\n[-]La socket a été fermée par le serveur !\n");
                    close(socket);
                    exit(-5);
                default:
                    //gestion du cas où le client veut se déconnecter avec le mot "quit"
                    if(strcmp(messageEnvoi, "/fin") == 0){
                        printf("\n[!]---fin de la discussion---[!]\n\n");
                        kill(getppid(), SIGTERM);
                        exit(0);
                    }
                    printf("Message %s envoyé avec succés ☻ (%d octets)\n",messageEnvoi,ecrits);
            }
        }
    }
    pthread_exit(0);
}


//fonction qui va servir au thread 
void * Recevoir(void * socketClient)
{
    // Reception des données du serveur
    int lus;
    // le message de la couche application!
    char * messageRecu = malloc(longueurMessage*sizeof(char));
    int socket = (long)socketClient;
    while (1)
    {
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        lus = read(socket, messageRecu, longueurMessage*sizeof(char));
        switch(lus)
        {
            case -1: 
                perror("read");
                close(socket);
                exit(-4);
            case 0:
                fprintf(stderr, "\n[-]La socket a été fermée par le serveur !\n");
                close(socket);
                exit(-5);
            default:
                //puts(messageRecu);
                printf("\n");
                //printf("%s \n", messageRecu);
                puts(messageRecu);
                // On a fini d'afficher le message recu on affiche la demande d'envoie
                puts("\n☼☼☼ Envoyer Un Message ☼☼☼");
                puts("→ ");
                
        }
    }
    pthread_exit(0);   
}

/*
void send_file(FILE *fp, int sockfd){
  int n;
  char data[longueurMessage] = {0};
 
  while(fgets(data, longueurMessage, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, longueurMessage);
  }
}
*/

int main(int argc, char *argv[]) 
{
    long socketClient;
    struct sockaddr_in pointDeRencontreDistant;
    socklen_t longueurAdresse;

    //  Creation des threads pour envoyer et recevoir des messages 
    pthread_t tEcouter;
    pthread_t tRecepteur;
    // Creation de thread pour la connexion
    pthread_t tConnexion;

    // Création d'un socket de communication
    // PF_INET c'est le domaine pour le protocole internet IPV4 
    socketClient = socket(PF_INET, SOCK_STREAM, 0);

    /* 0 indique que l’on utilisera leprotocole par défaut 
    associé à SOCK_STREAM soit TCP
    */

    // Teste la valeur renvoyée par l’appel système socket()
    // afin de vérifier la bonne création de cela
    if(socketClient < 0)
    {
        perror("socket echoué");
        exit(-1); // On sort en indiquant un code erreur
    }
    printf("la Socket a été bien créé ! (%ld)\n", socketClient);
    
    
    longueurAdresse = sizeof(pointDeRencontreDistant);
    // Initialise à 0 la struct sockaddr_in
    memset(&pointDeRencontreDistant, 0x00, longueurAdresse);

    // Renseigne la structure sockaddr_in avec les informations du serveur distant
    pointDeRencontreDistant.sin_family = PF_INET;
    pointDeRencontreDistant.sin_port =  htons(atoi(argv[2]));

    inet_aton(argv[1],&pointDeRencontreDistant.sin_addr);

    // Connection et gestion des erreurs
    /*
    connect() renvoie 0 s’il réussit, ou -1 s’il échoue, auquel cas errno contient lecode d’erreur
    */
       
    if((connect(socketClient, (struct sockaddr *)&pointDeRencontreDistant, longueurAdresse)) == -1)
    {
        perror("connect");
        close(socketClient); // on ferme la ressourece pour quitter
        exit(-2);
    }
    printf("Connection reussi avec the server \n");

    printf("status %d\n", status);
    connection(socketClient);
    printf("------------  [+]Connected to Server --------------------\n");
    /*
    FILE *fp;
    char *filename = "send.txt";
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("[-]Error in reading file.");
        exit(1);
    }
    send_file(fp, socketClient);
    printf("[+]File data sent successfully.\n");
    */
    pthread_create(&tRecepteur, NULL, Recevoir, (void *) socketClient);
    pthread_create(&tEcouter, NULL, Envoyer, (void *) socketClient);
    pthread_join(tEcouter, NULL);    
    pthread_join(tRecepteur, NULL);
    // On a fini on coupe la ressource pour quitter
    close(socketClient);
    return 0;
}