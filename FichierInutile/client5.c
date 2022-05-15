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
#define CHUNK_SIZE 512
#include <dirent.h>


#define longueurMessage 256

// Connecté ou pas!
int status = 0;

//Pour stocker l'adresse IP, qui va etre réutilisées par le thread d'envoi de fichiers
char * ip;

char * port;

char * filenameSEND;

int nonRecu;



// lister les commandes pour les mode de message possible
void Commandes()
{
    FILE * cmd;
    char c;
    cmd=fopen("Commande.txt","rt");
    while((c=fgetc(cmd))!=EOF){
        printf("%c",c);
    }
    fclose(cmd);
}


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
    DIR * d = opendir(dossier); 
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
    handle = open(filename, O_RDONLY );
    taille = lseek(handle, 0, SEEK_END );
    close(handle);
    return taille;
}


void * sendFile(void * SocketServer){
    long socketClient2;

    struct sockaddr_in servReceptionFichiersClient;
    socklen_t longueurAdr;


    socketClient2 = socket(PF_INET, SOCK_STREAM, 0);

    if(socketClient2 < 0){
        perror("[-] Création de la socket a échoué");
        exit(-1);
    }

    longueurAdr = sizeof(servReceptionFichiersClient);
    memset(&servReceptionFichiersClient, 0x00, longueurAdr);

    servReceptionFichiersClient.sin_family = PF_INET;
    servReceptionFichiersClient.sin_port =  htons(atoi(port));

    inet_aton(ip, &servReceptionFichiersClient.sin_addr);

    if(connect(socketClient2, (struct sockaddr *)&servReceptionFichiersClient, longueurAdr) == -1){
        perror("connect");
        close(socketClient2);
        exit(-2);
    }

    //Envoi du nom du fichier
    int writeName = write(socketClient2, filenameSEND, longueurMessage*sizeof(char));
    switch(writeName){
        case -1 : 
            perror("[-] Problème rencontré dans l'envoi du fichier au serveur\n");
            exit(-1);
        case 0 :
            perror("[-] La socket a été fermée par le serveur");
            exit(-1);
        default :
            printf("Le nom du fichier %s a été envoyé\n", filenameSEND);
    }

    char path[longueurMessage] = "fichiersClient/";
    strcat(path, filenameSEND); 

    //on recupere maintenant la taille du fichier
    int taille = tailleFile(path);
    char* tailleChar =  malloc( longueurMessage*sizeof(char));
    sprintf(tailleChar, "%d", taille);

    int writeTaille = write(socketClient2, tailleChar, longueurMessage*sizeof(char));
    //char integer[4];                 
    //*((int*)integer) = taille;         
    switch(writeTaille){
        case -1 :
            perror("[-] Problème dans l'envoi de la taille du fichier\n");
            exit(-1);
        case 0 :
            perror("[-] La connexion a été fermée par le serveur\n");
            exit(-1);
        default :
            printf("Taille du fichier envoyée avec succés\n");
    } 

    //On ouvre le fichier    
    FILE * fp = fopen(path,"r");
    
    if (fp == NULL) {
        perror("[-]Le fichier n'a pas pu être lu ou est introuvable ! \n");
        exit(1);
    }

    char data[1024];

    fread(data, taille, 1, fp);
    fclose(fp);

    int writeData = write(socketClient2, data, 1024);
    switch(writeData){
        case -1:
            perror("[-]Erreur rencontrée dans l'envoi du contenu du fichier");
            exit(-1);
        case 0:
            perror("La socket a été fermée par le serveur");
            exit(-1);
        default :
            printf("Contenu du fichier bien envoyé");
    }

}


void envoiFile(int socket, char * filename){
    //On recupere d'abord le nom du fichier
    //char * nameFile = (char*) malloc(20*sizeof(char));
    //printf("Quel fichier voulez-vous envoyer ?\n");
    //fgets(nameFile, 200*sizeof(char), stdin);
    //filename[strlen(filename)-1] = '\0';

    //Envoi du nom du fichier
    int writeName = write(socket, filename, longueurMessage*sizeof(char));
    switch(writeName){
        case -1 : 
            perror("[-] Problème rencontré dans l'envoi du fichier au serveur\n");
            exit(-1);
        case 0 :
            perror("[-] La socket a été fermée par le serveur");
            exit(-1);
        default :
            printf("Le nom du fichier %s a été envoyé\n", filename);
    }

    char path[longueurMessage] = "fichiersClient/";
    strcat(path, filename); 

    //on recupere maintenant la taille du fichier
    int taille = tailleFile(path);
    char* tailleChar =  malloc( longueurMessage*sizeof(char));
    sprintf(tailleChar, "%d", taille);

    int writeTaille = write(socket, tailleChar, longueurMessage*sizeof(char));
    //char integer[4];                 
    //*((int*)integer) = taille;         
    switch(writeTaille){
        case -1 :
            perror("[-] Problème dans l'envoi de la taille du fichier\n");
            exit(-1);
        case 0 :
            perror("[-] La connexion a été fermée par le serveur\n");
            exit(-1);
        default :
            printf("Taille du fichier envoyée avec succés\n");
    } 
    //int t = (long) integer;
    

    //char * chaineTaille = itoa(taille);
    /*int envoiTaille = write(socket, chaineTaille, sizeof(taille));
    if(envoiTaille == -1 || envoiTaille == 0){
        perror("Echec dans l'envoi de la du fichier");
        exit(-1);
    }*/
    //On ouvre le fichier

    
    FILE * fp = fopen(path,"r");
    
    if (fp == NULL) {
        perror("[-]Le fichier n'a pas pu être lu ou est introuvable ! \n");
        exit(1);
    }

    char data[1024];

    fread(data, taille, 1, fp);
    fclose(fp);

    int writeData = write(socket, data, 1024);
    switch(writeData){
        case -1:
            perror("[-]Erreur rencontrée dans l'envoi du contenu du fichier");
            exit(-1);
        case 0:
            perror("La socket a été fermée par le serveur");
            exit(-1);
        default :
            printf("Contenu du fichier bien envoyé");
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
            char ** fichiers = malloc(256*sizeof(char));
            int i , nombreFichiers, fichiersTrouver;
            char fichierChoisi[longueurMessage];
            memset(fichierChoisi, 0x00, longueurMessage*sizeof(char));   
            nombreFichiers = listeFichierDansDos("fichiersClient", fichiers);
            fichiersTrouver = 0;
            while(fichiersTrouver == 0){
                puts("\n☼☼☼ Choisissez un fichier ☼☼☼");
                printf("→ ");
                fgets(fichierChoisi, longueurMessage*sizeof(char),stdin);
                fichierChoisi[strlen(fichierChoisi) - 1]=0;
                for(i=0;i<nombreFichiers;i++)
                {     
                    if(strcmp(fichierChoisi,fichiers[i]) == 0)
                    {   

                        nonRecu = 1;
                        if(write(socket, messageEnvoi, strlen(messageEnvoi)) == -1){
                            perror("[-] Problème dans l'envoi du message 'file'");
                            exit(-1);
                        }
                        printf("Le message suivant a bien été envoyé : %s\n", messageEnvoi);
                        printf("Le fichier %s existe bien !\n",fichierChoisi);
                        fichiersTrouver = 1;
                        filenameSEND = fichierChoisi;
                        while(nonRecu){  }
                        pthread_t send;
                        pthread_create(&send, NULL, sendFile, NULL);
                    }
                }
                if(fichiersTrouver == 0)
                {
                    printf("! Ce fichier n'existe pas !\n");
                }
            }
        } else 
        {    
            ecrits = write(socket, messageEnvoi,strlen(messageEnvoi));
            switch(ecrits)
            {
                case -1: 
                    perror("[-] Erreur dans l'envoi");
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
                if(strcmp(messageRecu, "port") == 0){
                    int rcvPort = read(socket, port, longueurMessage*sizeof(char));
                    switch(rcvPort){
                        case -1:
                            perror("[-] Problème de reception du port sur lequel l'envoi du fichier va se faire\n");
                            exit(-1);
                        case 0:
                            perror("[-] La socket a été fermée par le serveur !\n");
                            exit(-1);
                        default:
                            printf("Reception du numero de port réussi avec succés %s ! \n", port);
                            nonRecu = 0;
                    }
                }
                else{
                    //puts(messageRecu);
                    printf("\n");
                    //printf("%s \n", messageRecu);
                    puts(messageRecu);
                    // On a fini d'afficher le message recu on affiche la demande d'envoie
                    puts("\n☼☼☼ Envoyer Un Message ☼☼☼");
                    puts("→ ");
                }                
        }
    }
    pthread_exit(0);   
}

int main(int argc, char *argv[]) 
{
    ip = argv[1];
    port = argv[2];
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
    

    pthread_create(&tRecepteur, NULL, Recevoir, (void *) socketClient);
    pthread_create(&tEcouter, NULL, Envoyer, (void *) socketClient);
    pthread_join(tEcouter, NULL);    
    pthread_join(tRecepteur, NULL);
    // On a fini on coupe la ressource pour quitter
    close(socketClient);
    return 0;
}