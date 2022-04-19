#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <pthread.h>
#include <unistd.h>
#include "liste.h"



#define longueurMessage 256

//tableau contenant les socket des deux clients à relayer 
// si socket == -1 → pas connecté
int * connecter;
int nombreClientConnecter;

// fermer toutes les sockets des clients connectés
void closeAllsockets( int * tab, int taille)
{
    int i;
    for(i = 0; i < taille; i++)
    {
        close(tab[i]);
        tab[i] = -1;
    }
}


//fonction qui va être utilisée par un thread du serveur pour 
//pouvoir transmettre les messages du client 1 au client 2
void * Relayer(void * SocketClient)
{   //on ecrase d'abord les données contenues dans messageEnvoi et messageRecu pour éviter d'avoir des données non désirables
    int socketClient = (long) SocketClient;
    char messageEnvoi[longueurMessage];
    char messageRecu[longueurMessage];
    int ecrits, lus;
    
    // On réceptionne les données du client (cf. protocole)
    while(1)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        //On lit le message envoyé par le client
        lus = read(socketClient,messageRecu,longueurMessage*sizeof(char));    
        switch(lus)
        {   //en cas d'erreur dans la lecture, -1 sera retourné par "lus"
            case -1: 
                perror("[-]Problem receiving the message");               
                closeAllsockets(connecter , nombreClientConnecter);
                exit(-5);
            case 0:
                fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                closeAllsockets(connecter , nombreClientConnecter);
            default:
                printf("Message receive by the client : %s (%d octets)\n\n",messageRecu,lus);
        }

        //On envoie des données vers le client (cf. protocole)   
        strcpy(messageEnvoi,messageRecu);
        int i;
        for(i = 0; i < nombreClientConnecter; i++)
        {
            if(socketClient != connecter[i])
            {
                ecrits = write(connecter[i], messageEnvoi,strlen(messageEnvoi));
                switch(ecrits)
                {
                    case -1: 
                        perror("[-]Problem of send the message");
                        closeAllsockets(connecter , nombreClientConnecter);
                        exit(-6);
                    case 0:
                        fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                        closeAllsockets(connecter , nombreClientConnecter);
                    default:
                        printf("Message %s envoyé avec succés (%d octets)\n\n",messageEnvoi,ecrits);
                }
            }
        }
    }
    closeAllsockets(connecter , nombreClientConnecter);
    pthread_exit(0);    
}

int main(int argc, char * argv[]) 
{

    int socketServeur;
    struct sockaddr_in pointDeRencontreLocal;
    socklen_t longueurAdresse;


    int socketDialogue;
    struct sockaddr_in pointDeRencontreDistant;
    int retour;

    connecter = malloc(10 * sizeof(int));

    //  Creation des threads pour envoyer et recevoir des messages 
    pthread_t tRelay;


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
    nombreClientConnecter = 0;
    while(1){
        printf("nombre de client connecté %d\n", nombreClientConnecter);  
        //Boucle d'attente de connexion: en théorie, un serveur attend indéfiniment
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
        
        connecter[nombreClientConnecter] = socketDialogue;  
        //une fois qu'on a réussi à connecter le client, on lance le thread qui va relayer les messages
        pthread_create(&tRelay, NULL, Relayer, (void *)(long) socketDialogue);   
        nombreClientConnecter++;    
    }
    
    close(socketServeur);
    return 0;
}