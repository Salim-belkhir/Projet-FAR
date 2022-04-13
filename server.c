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
    int socketServeur;
    struct sockaddr_in pointDeRencontreLocal;
    socklen_t longueurAdresse;

    int socketDialogue;
    struct sockaddr_in pointDeRencontreDistant;
    char messageEnvoi[longueurMessage];
    char messageRecu[longueurMessage];
    int ecrits, lus; 
    int retour;

    // Création d'un socket de communication
    // PF_INET c'est le domaine pour le protocole internet IPV4 
    socketServeur = socket(PF_INET, SOCK_STREAM, 0);
    /* 0 indique que l’on utilisera leprotocole par défaut 
    associé à SOCK_STREAM soit TCP
    */

    // Teste la valeur renvoyée par l’appel système socket()
    // afin de vérifier la bonne création de cela
    if(socketServeur < 0)
    {
        perror("socket not created");
        exit(-1); // On sort en indiquant un code erreur
    }
    printf("la Socket created successfully ! (%d)\n", socketServeur);
    
    
    longueurAdresse = sizeof(pointDeRencontreLocal);
    // Initialise à 0 la struct sockaddr_in
    memset(&pointDeRencontreLocal, 0x00, longueurAdresse);

    // Renseigne la structure sockaddr_in avec les informations du serveur distant
    pointDeRencontreLocal.sin_family = PF_INET;
    // Toutes les interfaces
    pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    pointDeRencontreLocal.sin_port =  htons(atoi(argv[2]));

    inet_aton(argv[1],&pointDeRencontreLocal.sin_addr);
    
    // On demande l'attachement local de la socket 
    if((bind(socketServeur,(struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
    {
        perror("bind");
        exit(-2);
    }

    printf("Socket attachée avec succés !\n");

    // maximum 5 dans la fille
    if(listen(socketServeur, 5) < 0)
    {
        perror("listen");
        exit(-3);
    }

    printf("Socket in listening! \n");

    // Boucle d'attente de connexion: en théorie, un  serveur attend indéfiniment à modifier avec des processus

    while(1)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        printf("Attente d'une demande de connexion (quitter avec Ctrl-C) \n\n");
        // c'est un appel bloquant 
        socketDialogue = accept(socketServeur, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);

        if(socketDialogue < 0)
        {
            perror("accept");
            close(socketDialogue);
            close(socketServeur);
            exit(-4);
        }

        // On réception les données du client (cf. protocole)
        lus = read(socketDialogue,messageRecu,longueurMessage*sizeof(char));

        switch(lus)
        {
            case -1: 
                perror("read");
                close(socketDialogue);
                exit(-5);
            case 0:
                fprintf(stderr, "La socket a été fermée par le client !\n\n");
                close(socketDialogue);
                return 0;
            default:
                printf("Message recu du client : %s (%d octets)\n\n",messageRecu,lus);
        }

        //On envoie des données vers le client (cf. protocole)
        sprintf(messageEnvoi, "ok\n");    
        ecrits = write(socketDialogue, messageEnvoi,strlen(messageEnvoi));
        
        switch(ecrits)
        {
            case -1: 
                perror("write");
                close(socketDialogue);
                exit(-6);
            case 0:
                fprintf(stderr, "La socket a été fermée par le client !\n\n");
                close(socketDialogue);
                return 0;
            default:
                printf("Message %s envoyé avec succés (%d octets)\n\n",messageEnvoi,ecrits);
        }

        // on ferme la socket de dialogue et on se replace en attente
        close(socketDialogue);
    }

    close(socketServeur);
    return 0;
}