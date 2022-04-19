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

//tableau contenant les socket des deux clients à relayer 
// si socket == -1 → pas connecté
int connec[2] = {-1,-1};

// pour continuer la communication
int continuer = 1;


//fonction d'extraction 

char * extraire(char * message){
    int pasFini = 1;
    int i = 0;
    char * msg = malloc(25*sizeof(char));
    while(pasFini){
        if(message[i] == ' ' ){
            pasFini = 0;
        }
        else{
            msg[i] = message[i];
            i++;
        }
    }
    return msg;
}


//fonction pour pouvoir récupérer une chaîne de charactères

char * get(char * message, int deb, int fin){
    char * msg = (char*) malloc((fin - deb + 1)*sizeof(char));
    int taille = strlen(message);
    if(fin > taille){
        perror("[!] L'indice de fin n'est pas valable, la chaîne de caractères est plus vide que ça");
        exit(-1);
    }
    int i = 0;
    for(deb ; deb <= fin; deb++){
        msg[i] = message[deb];
        i++;
    }
    return msg;
}


//fonction qui va être utilisée par un thread du serveur pour 
//pouvoir transmettre les messages du client 1 au client 2
void * Relayer(void * tour)
{   //on ecrase d'abord les données contenues dans messageEnvoi et messageRecu pour éviter d'avoir des données non désirables
    int sendToclient = (long) tour;
    char messageEnvoi[longueurMessage];
    char messageRecu[longueurMessage];
    int ecrits, lus;
    if(sendToclient == 0 ){
        sendToclient ++;
    } else sendToclient --;

    // On réceptionne les données du client (cf. protocole)
    while(continuer == 1)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        //On lit le message envoyé par le client
        lus = read(connec[(long) tour],messageRecu,longueurMessage*sizeof(char));
    
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

        //verification de si c'est un message spécial
        
        if(strcmp(get(messageRecu, 0, 3), "/fin") == 0){
            //close(socket);
            //close(socketServeur);
            printf("La discussion est finie\n");
            break;
        }
        else{
            if(messageRecu[0] == '@'){
                    //pour envoyer un message directement à quelqu'un
                    char * destinataire = extraire(get(messageRecu, 1, strlen(messageRecu)-1));
                    //on réutilise son nom pour parcourir la liste  
                    printf("destinataire saisi : %s\n", destinataire);
                    break;
            }
            else{
                if(messageRecu[0] == '!'){
                    //pour envoyer un message urgent à quelqu'un
                    char * destinataire = extraire(get(messageRecu, 1, strlen(messageRecu)-1));
                    //on réutilise son nom pour parcourir la liste et le retrouver 
                    printf("destinataire saisi : %s\n", destinataire);
                    break;
                }
            }
        } 
                
        
        

        

        if( strcmp(messageRecu, "fin") != 0)
        {
        //On envoie des données vers le client (cf. protocole)   
            strcpy(messageEnvoi,messageRecu);
            ecrits = write(connec[sendToclient], messageEnvoi,strlen(messageEnvoi));
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
        } else {
            continuer = 0;
        }
    }
    connec[0] = -1;
    connec[1] = -1;
    close(connec[0]);
    close(connec[1]);
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

            connec[i] = socketDialogue;
            i++; 
        }
        //une fois qu'on a réussi à connecter les deux clients, on lance les deux threads qui vont relayer les messages
        long client1 = 0;
        long client2 = 1;
        pthread_create(&tRelay1, NULL, Relayer, (void *) client1); 
        pthread_create(&tRelay2, NULL, Relayer, (void *) client2);
        pthread_join(tRelay1, NULL);    
        pthread_join(tRelay2, NULL);
    }
    close(socketServeur);
    return 0;
}