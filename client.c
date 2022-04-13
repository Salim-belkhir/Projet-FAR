#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define longueurMessage 256

int main(int argc, char *argv[]) 
{
    int socketClient;
    struct sockaddr_in pointDeRencontreDistant;
    socklen_t longueurAdresse;

    // le message de la couche application ! 
    char messageEnvoi[longueurMessage];
    char messageRecu[longueurMessage];

    //  nb d'octets ecrits et lus  
    int ecrits, lus; 
    int retour;

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
    printf("la Socket a été bien créé ! (%d)\n", socketClient);
    
    
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

    // Initialisation à 0 les messages 
    memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
    memset(messageRecu, 0x00, longueurMessage*sizeof(char));

    // Envoie un message au serveur et gestion des erreurs
    //sprintf(messageEnvoi, "Holla");
    
    printf("envoyez un message : ");
    scanf("%s", &messageEnvoi);

    ecrits = write(socketClient, messageEnvoi,strlen(messageEnvoi));

    switch(ecrits)
    {
        case -1: 
            perror("write");
            close(socketClient);
            exit(-3);
        case 0:
            fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
            close(socketClient);
            return 0;
        default:
            printf("Message %s envoyé avec succés (%d octets)\n\n",messageEnvoi,ecrits);
    }

    // Reception des données du serveur 
    lus = read(socketClient, messageRecu, longueurMessage*sizeof(char));
    switch(lus)
    {
        case -1: 
            perror("read");
            close(socketClient);
            exit(-4);
        case 0:
            fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
            close(socketClient);
            return 0;
        default:
            printf("Message recu du serveur : %s (%d octets)\n\n",messageRecu,lus);
    }

    // On a fini on coupe la ressource pour quitter
    close(socketClient);

    return 0;
}