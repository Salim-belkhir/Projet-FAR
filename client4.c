#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <pthread.h>
#include <unistd.h>

#define longueurMessage 256

// Connecté ou pas!
int status = 0;

void * connection(void * socketClient)
{
    int ecrits;
    int socket = (long)socketClient;
    // le message de la couche application ! 
    char messageEnvoi[longueurMessage];
    // Envoie un message au serveur et gestion des erreurs
    //sprintf(messageEnvoi, "Holla");
    memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
    puts("\n↔↔↔ Choisissez un pseudo ↔↔↔");
    printf("→ ");
    fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
    messageEnvoi[strlen(messageEnvoi) - 1]=0;
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
            printf("Pseudo %s envoyé avec succés (%d octets)\n",messageEnvoi,ecrits);
    }
    
    pthread_exit(0);
}

//fonction qui va servir au thread pour l'envoi de messages
void * Envoyer(void * socketClient)
{
    int ecrits;
    int socket = (long)socketClient;
    // le message de la couche application ! 
    char messageEnvoi[longueurMessage];
    // Envoie un message au serveur et gestion des erreurs
    //sprintf(messageEnvoi, "Holla");
    while(1)
    {  
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        puts("\n↔↔↔ Envoyer Un Message ↔↔↔");
        printf("→ ");
        fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
        messageEnvoi[strlen(messageEnvoi) - 1]=0;
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
                printf("Message %s envoyé avec succés (%d octets)\n",messageEnvoi,ecrits);
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
    char messageRecu[longueurMessage];
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
                if(status == 0)
                {
                    if(strcmp(messageRecu, "valide") == 0)
                    {
                        status = 1;
                        printf("Le pseudo choisit est bien valide \n");
                        pthread_exit(0);
                        break;
                    }else if(strcmp(messageRecu, "invalide") == 0)
                    {
                        printf("Le pseudo choisit exist déja !!\n");
                    }
                } else 
                {
                    //puts(messageRecu);
                    printf("\nMessage recu de l'autre Client → ");
                    puts(messageRecu);
                    // On a fini d'afficher le message recu on affiche la demande d'envoie
                    puts("\n↔↔↔ Envoyer Un Message ↔↔↔");
                    puts("→ ");
                }
        }
    }
    pthread_exit(0);   
}

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
    while(status == 0)
    {
        pthread_create(&tConnexion, NULL, connection, (void *) socketClient);
        pthread_create(&tRecepteur, NULL, Recevoir, (void *) socketClient);   
        pthread_join(tConnexion, NULL);    
    }
    pthread_create(&tRecepteur, NULL, Recevoir, (void *) socketClient);
    pthread_create(&tEcouter, NULL, Envoyer, (void *) socketClient);
    pthread_join(tEcouter, NULL);    
    pthread_join(tRecepteur, NULL);
    // On a fini on coupe la ressource pour quitter
    close(socketClient);
    return 0;
}