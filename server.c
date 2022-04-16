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

//tableau contenant les deux clients
int connec[2] = {-1,-1};


//fonction qui va être utilisée par un thread du serveur pour 
//pouvoir transmettre les messages du client 1 au client 2
void * Relayer1()
{   //on ecrase d'abord les données contenues dans messageEnvoi et messageRecu pour éviter d'avoir des données non désirables
    char messageEnvoi[longueurMessage];
    char messageRecu[longueurMessage];
    int ecrits, lus; 
    // On réceptionne les données du client (cf. protocole)
    while(1)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        //On lit le message envoyé par le client
        lus = read(connec[0],messageRecu,longueurMessage*sizeof(char));
        switch(lus)
        {   //en cas d'erreur dans la lecture, -1 sera retourné par "lus"
            case -1: 
                perror("[-]Problem receiving the message");               
                close(connec[0]);
                close(connec[1]);
                exit(-5);
            case 0:
                fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                close(connec[0]);
                close(connec[1]);
            default:
                printf("Message receive by the client : %s (%d octets)\n\n",messageRecu,lus);
        }

        //On envoie des données vers le client (cf. protocole)    
        sprintf(messageEnvoi,messageRecu);
        ecrits = write(connec[1], messageEnvoi,strlen(messageEnvoi));
        switch(ecrits)
        {
            case -1: 
                perror("[-]Problem of send the message");
                close(connec[0]);
                close(connec[1]);
                exit(-6);
            case 0:
                fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                close(connec[0]);
                close(connec[1]);
            default:
                printf("Message %s envoyé avec succés (%d octets)\n\n",messageEnvoi,ecrits);
        }
    }
}


//fonction qui va être utilisée par un thread du serveur pour 
//pouvoir transmettre les messages du client 2 au client 1 cette fois-ci
void * Relayer2()
{
    char messageEnvoi[longueurMessage];
    char messageRecu[longueurMessage];
    int ecrits, lus; 
    
    // On réceptionne les données du client (cf. protocole)
    while(1)
    {   //on ecrase d'abord les données contenues dans messageEnvoi et messageRecu pour éviter d'avoir des données non désirables
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        //On lit le message envoyé par le client
        lus = read(connec[1],messageRecu,longueurMessage*sizeof(char));
        switch(lus)
        {
            //en cas d'erreur dans la lecture, -1 sera retourné par "lus"
            case -1: 
                perror("[-]Problem receiving the message");
                close(connec[0]);
                close(connec[1]);
                exit(-5);
            case 0:
                fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                close(connec[0]);
                close(connec[1]);
            default:
                printf("Message recu du client : %s (%d octets)\n\n",messageRecu,lus);
        }

        //On envoie des données vers le client (cf. protocole)    
        sprintf(messageEnvoi,messageRecu);

        ecrits = write(connec[0], messageEnvoi,strlen(messageEnvoi));
        switch(ecrits)
        {
            case -1: 
                perror("write");
                close(connec[0]);
                close(connec[1]);
                exit(-6);
            case 0:
                fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                close(connec[0]);
                close(connec[1]);
            default:
                printf("Message %s envoyé avec succés (%d octets)\n\n",messageEnvoi,ecrits);
        }
    }
}

int main(int argc, char *argv[]) 
{
    int socketServeur;
    struct sockaddr_in pointDeRencontreLocal;
    socklen_t longueurAdresse;

    int socketDialogue;
    struct sockaddr_in pointDeRencontreDistant;
    int retour;

    //  Creation des threads pour envoyer et recevoir des messages 
    pthread_t tRelay1;
    pthread_t tRelay2;


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
        perror("[-]Socket not created");
        exit(-1); // On sort en indiquant un code erreur
    }
    printf("[+]Socket created successfully ! (%d)\n", socketServeur);
    
    
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
        perror("[-]Problem of binding");
        exit(-2);
    }

    printf("[+]Socket attached with success!\n");

    // maximum 5 clients dans la fille
    if(listen(socketServeur, 5) < 0)
    {
        perror("[-]The server can not listen");
        exit(-3);
    }

    printf("Server on listening! \n");
    int i ;
    while(1){
        //Boucle d'attente de connexion: en théorie, un serveur attend indéfiniment
        i = 0;
        while( i < 2)
        {
            //Dans un premier temps, il faut s'assurer qu'on a bien deux clients qui vont se connecter
            //on va d'abord rester dans cette boucle, tant que deux clients ne se sont pas bien connectés
            printf("Attente d'une demande de connexion (quitter avec Ctrl-C) \n\n");
            // c'est un appel bloquant 
            socketDialogue = accept(socketServeur, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
            if(socketDialogue < 0)
            {
                perror("[-]We can not connect the client");
                close(socketDialogue);
                close(socketServeur);
                exit(-4);
            }
            printf("%d\n", socketDialogue);
            connec[i] = socketDialogue;
            i++; 
        }
        //une fois qu'on a réussi à connecter les deux clients, on lance les deux threads qui vont relayer les messages
        pthread_create(&tRelay1, NULL, Relayer1, NULL); 
        pthread_create(&tRelay2, NULL, Relayer2, NULL);
        pthread_join(tRelay1, NULL);    
        pthread_join(tRelay2, NULL);

    }
    close(socketServeur);
    return 0;
}