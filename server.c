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
#include <signal.h>

#define longueurMessage 10000

// liste contenant les socket des  clients à relayer 
// si socket == -1 → pas connecté
liste * utilisateurConnecter;
int nombreClientConnecter;
pthread_mutex_t mutex;

/**
 * @brief 
 * Create a new file.
 * Receives the data from the client.
 * Write the data into the file
 * @param sockfd 
 */
void write_file(int sockfd){
  int n;
  FILE *fp;
  char *filename = "recv.txt";
  char buffer[longueurMessage];
 
  fp = fopen(filename, "w");
  while (1) {
    n = recv(sockfd, buffer, longueurMessage, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, longueurMessage);
  }
  return;
}

// fermer toutes les sockets des clients connectés
void closeAllsockets()
{
    if (liste_est_vide(utilisateurConnecter))
    {
        exit(EXIT_FAILURE);
    }
    Element * actuel = utilisateurConnecter -> premier;
    while (actuel-> suivant != NULL)
    {
        close(actuel -> id);
        actuel = actuel->suivant;
    }
}

//fonction pour pouvoir récupérer une chaîne de charactères
char ** Separation(char * message){
    
    // Allocation de la memoire
    char ** msg = malloc(longueurMessage*sizeof(char *));
    int i;
    for(i=0; i<3;i++)
    {
        msg[i] = malloc(longueurMessage*sizeof(char));
    }

    // La définitions de séparateurs connus.
    const char * separators = " ";
    
    // On cherche à récupérer, un à un, tous les mots (token) de la phrase
    // et on commence par le premier.
    char * strToken = malloc(longueurMessage*sizeof(char));
    strToken = strtok ( message, separators );
    
    i = 0;
    while ( strToken != NULL ) {
        //printf (" → %s\n", strToken);
        msg[i] = strToken;
        // On demande le token suivant.
        if(i < 1)
        {
            strToken = strtok( NULL, separators );
        } else {
            strToken = strtok( NULL, "");
        }
        i++;
    }

    /*
    for(i = 0 ; i < 3; i++)
    {
        printf("msg[%d] → %s\n", i, msg[i]);
    }
    */
    //// le caractères spécial
    //printf("le caractère est → %s \n", msg[0]);
    //
    //// Utilisateur qui recoie le message
    //printf("l'utilisateur est → %s \n", msg[1]);

    //// Message qui sera envoyer
    //printf("le message est → %s \n", msg[2]);
    ////int taille = strlen(message);
    return msg;
}

// La fonction recupere un pseudo d'un utilisateur s'il est connecté apartir de son id
// renvoie le pseudo
char *  pseudoParID(int id){
    char *  result = malloc(longueurMessage*sizeof(char));
    Element * actuel = utilisateurConnecter -> premier;
    while(actuel -> suivant != NULL)
    {
        if(actuel -> id == id)
        {
            strcpy(result,actuel->pseudo);
        }
        actuel = actuel->suivant;
    }
    return result;
}

// lister les commandes pour les mode de message possible
char * Commandes()
{
    FILE * cmd;
    int i = 0;
    char c;
    char * listeCommandes = malloc(longueurMessage*sizeof(char));
    cmd=fopen("Commande.txt","rt");
    while((c=fgetc(cmd))!=EOF){
        listeCommandes[i] = c;
        i++;
    }
    fclose(cmd);
    return listeCommandes;
}


void EnvoyerMessageSpe(int socketClient, char * Message, char * client, char * commandeSpecial)
{

    int ecrits;
    char * messageEnvoi = malloc(longueurMessage*sizeof(char));
    Element * actuel = utilisateurConnecter-> premier;
    while(actuel -> suivant != NULL)
    {
        if(socketClient != actuel -> id && strcmp(client, actuel -> pseudo) == 0)  
        {
            printf("on rentre dans la condition \n");
            if(strcmp(commandeSpecial, "/fin") == 0)
            {
                supprimer_val(utilisateurConnecter, socketClient);                
                printf("nombreClientConnecter %d\n", liste_taille(utilisateurConnecter));
                afficherListe(utilisateurConnecter);
                close(socketClient);
            } 
            if (strcmp(commandeSpecial,"@") == 0)
            {
                printf("on vas cree le @ message \n");
                strcat(messageEnvoi,"@");
                strcat(messageEnvoi,pseudoParID(socketClient));
                strcat(messageEnvoi," vous a envoyé ce message : \n");
                strcat(messageEnvoi,"→ ");
                strcat(messageEnvoi, Message);
                printf("le @ message est %s\n", messageEnvoi);
            }
            else if(strcmp(commandeSpecial,"!") == 0)
            {
                strcat(messageEnvoi,"! MESSAGE URGENT ! \n");
                strcat(messageEnvoi,"!");
                strcat(messageEnvoi,pseudoParID(socketClient));
                strcat(messageEnvoi," vous a envoyé ce message urgent : \n");
                strcat(messageEnvoi,"→ ");
                strcat(messageEnvoi, Message);
            }
            else if (strcmp(commandeSpecial,"/mp") == 0)
            {
                strcat(messageEnvoi,"♦ MESSAGE PRIVE ♦\n");
                strcat(messageEnvoi,"♦");
                strcat(messageEnvoi,pseudoParID(socketClient));
                strcat(messageEnvoi," vous a envoyé ce message privé : \n");
                strcat(messageEnvoi,"→ ");
                strcat(messageEnvoi, Message);  
            }    
            printf("le message à envoyé %s",messageEnvoi);
            ecrits = write(actuel -> id, messageEnvoi,strlen(messageEnvoi));
            switch(ecrits)
            {
                case -1: 
                    perror("[-]Problem of send the message");
                    closeAllsockets(utilisateurConnecter);
                    exit(-6);
                case 0:
                    fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                    closeAllsockets(utilisateurConnecter);
                default:
                    printf("Message %s envoyé avec succés (%d octets)\n\n",Message,ecrits);
            }
        } 
        actuel = actuel->suivant;
    }
}

void EnvoyerMessage(int socketClient, char * Message)
{
    int ecrits;
    char * messageEnvoi = malloc(longueurMessage*sizeof(char));
    strcat(messageEnvoi,"▬▬▬ MESSAGE A TOUT LE MONDE ▬▬▬\n");
    strcat(messageEnvoi,"▬ ");
    strcat(messageEnvoi,pseudoParID(socketClient));
    strcat(messageEnvoi," ▬");
    strcat(messageEnvoi," a envoyé ce message à tout les membres : \n");
    strcat(messageEnvoi,"→ ");
    strcat(messageEnvoi, Message); 

    Element * actuel = utilisateurConnecter -> premier;
    while(actuel -> suivant != NULL && liste_taille(utilisateurConnecter) > 0)
    {
        if(socketClient != actuel -> id)
        {
            ecrits = write(actuel -> id, messageEnvoi,strlen(messageEnvoi));
            switch(ecrits)
            {
                case -1: 
                    perror("[-]Problem of send the message");
                    closeAllsockets(utilisateurConnecter);
                    exit(-6);
                case 0:
                    fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                    closeAllsockets(utilisateurConnecter);
                default:
                    printf("Message %s envoyé avec succés (%d octets)\n\n",Message,ecrits);
            }
        } 
        actuel = actuel->suivant;
    }
}

void reponseClient(int socketClient, char * Message)
{
    int ecrits;
    ecrits = write(socketClient, Message,strlen(Message));
    switch(ecrits)
    {
        case -1: 
            perror("[-]Problem of send the message");
            closeAllsockets(utilisateurConnecter);
            exit(-6);
        case 0:
            fprintf(stderr, "[!]The socket was closed by the client !\n\n");
            closeAllsockets(utilisateurConnecter);
        default:
            printf("Message %s envoyé avec succés (%d octets)\n\n",Message,ecrits);
    }
} 
 

// La fonction vérifie si un pseudo est disponible donc aucun client ne la choisit avant
// renvoie si il n'existe pas 1 sinon
int existPseudo(char * pseudo) {
    int result = 0;
    Element * actuel = utilisateurConnecter -> premier;
    while(actuel -> suivant != NULL && result == 0)
    {
        if(strcmp(pseudo, actuel -> pseudo) == 0)
        {
            result = 1;
        }
        actuel = actuel->suivant;
    }
    return result;
}

//fonction qui va être utilisée par un thread du serveur pour 
//pouvoir transmettre les messages du client 1 au client 2
void * Relayer(void * SocketClient)
{   //on ecrase d'abord les données contenues dans messageEnvoi et messageRecu pour éviter d'avoir des données non désirables
    int socketClient = (long) SocketClient;
    char * messageEnvoi = malloc(longueurMessage* sizeof(char));
    char * messageRecu = malloc(longueurMessage*sizeof(char));
    int lus;
    // On réceptionne les données du client (cf. protocole)
    int i = 0;
    while(1)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));

        //On lit le message envoyé par le client
        lus = read(socketClient,messageRecu,longueurMessage*sizeof(char));    
        switch(lus)
        {   //en cas d'erreur dans la lecture, -1 sera retourné par "lus"
            case -1: 
                perror("[-]Probleme à la reception du message !");               
                closeAllsockets(utilisateurConnecter);
                exit(-5);
            case 0:
                fprintf(stderr, "[!]La socket a été fermé par le client !\n\n");
                closeAllsockets(utilisateurConnecter);
            default:
                if(i < 1)
                {                      
                    printf("▬▬▬ %s ▬▬▬\n",messageRecu);
                    if(strcmp(messageRecu, "tentative de connexion") != 0)
                    {
                        printf("Pseudo recu du client : %s (%d octets)\n\n",messageRecu,lus);
                        if (existPseudo(messageRecu) == 0)
                        {
                            strcpy(messageEnvoi,"valide");
                            reponseClient(socketClient, messageEnvoi);                            
                            printf("le pseudo choisit est valide!\n");
                            printf("on ajoute : %d a la file\n", socketClient);
                            char * pseudo = malloc(longueurMessage* sizeof(char));
                            strcpy(pseudo, messageRecu);
                            printf("on ajoute le pseudo : %s\n",pseudo);
                            ajouter_debut(utilisateurConnecter,socketClient, pseudo);
                            printf("taille apres l'ajout : %d \n",liste_taille(utilisateurConnecter));
                            afficherListe(utilisateurConnecter);  
                            //une fois qu'on a réussi à connecter le client, on lance le thread qui va relayer les messages
                            nombreClientConnecter = liste_taille(utilisateurConnecter);              
                            // chaque client à son propre identifiant 
                            i++;
                        } else 
                        {
                            strcpy(messageEnvoi,"invalide");
                            reponseClient(socketClient, messageEnvoi);        
                            printf("le pseudo choisit existe déja donc il faut choisir un autre!\n");
                        }
                    } else {
                        strcpy(messageEnvoi,"Choisissez un pseudo");
                        reponseClient(socketClient, messageEnvoi);  
                    }
            }else {
                if (i <= 1)
                {
                    i++;
                }
                printf("Message recu du client : %s (%d octets)\n\n",messageRecu,lus);
            } 
        }

        if(i >= 2) 
        {
            // verification de si c'est un message spécial
            // et l'envoie des données vers les clients (cf. protocole)   
            char ** separation;
            char * message = malloc(longueurMessage*sizeof(char));
            strcpy(message,messageRecu);
            separation = Separation(messageRecu);
        
            if(strcmp(separation[0], "/fin") == 0)
            {
                //déconnexion côté client, on le supprime de la liste des utilisateurs connectés
                printf("La discussion est finie avec le client numéro : %d\n", socketClient);
                supprimer_val(utilisateurConnecter, socketClient);
                nombreClientConnecter = liste_taille(utilisateurConnecter);
                printf("La taille actuelle après déconnexion est de : %d\n", liste_taille(utilisateurConnecter));
                afficherListe(utilisateurConnecter);
                close(socketClient);
                pthread_exit(0);
            }
            else if(strcmp(separation[0],"@") == 0)
            {
                printf("on est dans @ case \n");
                //printf("le client %s\n",separation[1]);                
                //printf("le message %s\n", separation[2]);
                strcpy(messageEnvoi, separation[2]);    
                printf("le message d'envoie est %s \n", messageEnvoi);
                pthread_mutex_lock(&mutex);
                EnvoyerMessageSpe(socketClient, messageEnvoi, separation[1], separation[0]);        
                pthread_mutex_unlock(&mutex);
            }
            else if(strcmp(separation[0],"/mp") == 0)
            {
                printf("on est dans le message privé case \n");
                //printf("le client %s\n",separation[1]);                
                //printf("le message %s\n", separation[2]);
                strcpy(messageEnvoi, separation[2]);    
                pthread_mutex_lock(&mutex);
                    EnvoyerMessageSpe(socketClient, messageEnvoi, separation[1], separation[0]);        
                pthread_mutex_unlock(&mutex);
            }
            else if(strcmp(separation[0],"!") == 0)
            {
                printf("on est dans ! case \n");
                //printf("le client %s\n",separation[1]);                
                //printf("le message %s\n", separation[2]);
                strcpy(messageEnvoi, separation[2]);    
                pthread_mutex_lock(&mutex);
                    EnvoyerMessageSpe(socketClient, messageEnvoi, separation[1], separation[0]);        
                pthread_mutex_unlock(&mutex);
                /* Urgent */
            }
            else if(strcmp(separation[0],"/help") == 0)
            {
                printf("on est dans /help case \n");
                //printf("le client %s\n",separation[1]);                
                //printf("le message %s\n", separation[2]);
                strcpy(messageEnvoi, Commandes());    
                reponseClient(socketClient, Commandes());     
            }
            else if(strcmp(separation[0], "file") == 0)
            {
                printf("on est file case \n");
            }
            else
            {
                printf("on est dans default case \n");
                strcpy(messageEnvoi,message);    
                pthread_mutex_lock(&mutex);
                    EnvoyerMessage(socketClient, messageEnvoi);        
                pthread_mutex_unlock(&mutex);
            }
        }         
    }
    closeAllsockets(utilisateurConnecter);
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
    
    utilisateurConnecter = cree_liste();
    pthread_mutex_init(&mutex, NULL);
    //  Creation des threads pour envoyer et recevoir des messages 
    pthread_t tRelay;

    // Création d'un socket de communication
    // PF_INET c'est le domaine pour le protocole internet IPV4 
    socketServeur = socket(PF_INET, SOCK_STREAM, 0);
    /* 0 indique que l’on utilisera le protocole par défaut 
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
    if(listen(socketServeur, 10) < 0)
    {
        perror("[-]The server can not listen");
        exit(-3);
    }
    
    while(1){
        printf("[+]Serveur En Ecoute! \n");
        printf("nombre de clients connectés %d\n", liste_taille(utilisateurConnecter));  
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
        /*
        char buffer[longueurMessage];
        write_file(socketDialogue);
        printf("[+]Data written in the file successfully.\n");
        */

        //printf("on ajoute : %d a la file\n", socketDialogue);
        //identifierClient = socketDialogue;
        //if(nombreClientConnecter == 0) ajouter_debut(utilisateurConnecter, identifierClient,  pseudo );
        //else ajouter_fin(utilisateurConnecter,identifierClient, pseudo );
        //printf("taille : %d \n",Taille(utilisateurConnecter));  
        //une fois qu'on a réussi à connecter le client, on lance le thread qui va relayer les messages
        pthread_create(&tRelay, NULL, Relayer, (void *)(long) socketDialogue);
        //nombreClientConnecter = Taille(utilisateurConnecter);              
        // chaque client à son propre identifiant 
    }    
    close(socketServeur);
    return 0;
}