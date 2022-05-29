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
#include <dirent.h>
#include "channel2.h"


#define longueurMessage 10000
#define sizeofArraySalons 10


/******   Les variables globales    ******/

Channel * salons[sizeofArraySalons];

int nombreSalons;

char * ip;   //adresse IP

int port;  //numéro de port de connexion


 //structure liste contenant la liste des clients connectés, pour chaque client on enregistre sa socket(comme id) et son pseudo 
// si socket == -1 → pas connecté
liste * utilisateursConnectes; 

int nombreClientsConnectes;

pthread_mutex_t mutex;

pthread_mutex_t mutexPort;

/****************************************/


/**
 * @brief 
 * ferme toutes les sockets des clients connectés
 */
void closeAllsockets()
{
    if (liste_est_vide(utilisateursConnectes))
    {
        exit(EXIT_FAILURE);
    }
    Element * actuel = utilisateursConnectes -> premier;
    while (actuel-> suivant != NULL)
    {
        close(actuel -> id);
        actuel = actuel->suivant;
    }
}



/**
 * @brief 
 * fonction pour pouvoir récupérer une chaîne de charactères
 * @param message Chaine de charactères de laquelle on doit extraire une partie
 * @return char** 
 */
char ** Separation(char * message){

    // Allocation de la memoire
    char ** msg = malloc(longueurMessage*sizeof(char *));
    int i;
    for(i=0; i<3;i++)
    {
        msg[i] = malloc(longueurMessage*sizeof(char));
    }

    // La définition de séparateurs connus
    const char * separators = " ";
    
    // On cherche à récupérer, un à un, tous les mots (token) de la phrase et on commence par le premier
    char * strToken = malloc(longueurMessage*sizeof(char));
    strToken = strtok ( message, separators );
    
    i = 0;
    while ( strToken != NULL ) {
        msg[i] = strToken;
        // On demande le token suivant
        if(i < 1)
        {
            strToken = strtok( NULL, separators );
        } else {
            strToken = strtok( NULL, "");
        }
        i++;
    }
    return msg;
}


/**
 * @brief 
 *  La fonction recupère un pseudo d'un utilisateur s'il est connecté à partir de son id
 * @param id Identifiant du client (l'entier retourné par sa socket également)
 * @return char*  renvoie le pseudo
 */
char *  pseudoParID(int id){
    char *  result = malloc(longueurMessage*sizeof(char));
    Element * actuel = utilisateursConnectes -> premier;
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


/**
 * @brief 
 * liste les commandes pour les différents modes de message possibles
 * @return char* 
 */
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

/**
 * @brief 
 * fonction qui permet d'envoyer un message spécial en fonction du message reçu par le client
 * @param socketClient socket du client qui envoie le message
 * @param Message message à envoyer
 * @param client pseudo du client à qui il faut envoyer le message
 * @param commandeSpecial la commande spéciale saisie avant le message
 */
void EnvoyerMessageSpe(int socketClient, char * Message, char * client, char * commandeSpecial)
{
    int ecrits;
    char * messageEnvoi = malloc(longueurMessage*sizeof(char));
    Element * actuel = utilisateursConnectes-> premier;
    while(actuel -> suivant != NULL)
    {
        if(socketClient != actuel -> id && strcmp(client, actuel -> pseudo) == 0)  
        {
            if(strcmp(commandeSpecial, "/fin") == 0)
            {
                supprimer_val(utilisateursConnectes, socketClient);                
                printf("nombreClientsConnectes %d\n", liste_taille(utilisateursConnectes));
                afficherListe(utilisateursConnectes);
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
                    closeAllsockets(utilisateursConnectes);
                    exit(-6);
                case 0:
                    fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                    closeAllsockets(utilisateursConnectes);
                default:
                    printf("Message %s envoyé avec succés (%d octets)\n\n",Message,ecrits);
            }
        } 
        actuel = actuel->suivant;
    }
}

/**
 * @brief 
 * fonction qui envoie un message envoyé par un client à tous les autres
 * @param socketClient socket du client qui envoie le message
 * @param Message message qu'il souhaite envoyé
 */
void EnvoyerMessage(int socketClient, char * Message)
{   
    //On récupére le channel auquel le client est attaché
    char * salon = malloc(100*sizeof(char));
    strcpy(salon,getCanalClient(utilisateursConnectes, socketClient));
    int ecrits;
    char * messageEnvoi = malloc(longueurMessage*sizeof(char));
    memset(messageEnvoi, 0, longueurMessage*sizeof(char)); 
    strcat(messageEnvoi,"▬▬▬ MESSAGE A TOUT LE CHANNEL ▬▬▬\n");
    strcat(messageEnvoi,"▬ ");
    strcat(messageEnvoi, pseudoParID(socketClient));
    strcat(messageEnvoi," ▬");
    strcat(messageEnvoi," a envoyé ce message à tous les membres du canal '");
    strcat(messageEnvoi,salon);
    strcat(messageEnvoi, "' : \n");
    strcat(messageEnvoi,"→ ");
    strcat(messageEnvoi, Message); 

    int i = 0;
    int dontFind = 1;
    //On cherche le salon qui est associé au client
    while(i < 3 && dontFind){
        if(strcmp(salons[i]->nom,salon) == 0){
            //On a trouvé la position du channel dans le tableau de salons
            dontFind = 0;
        } else {
            i++;
        }
    }

    printf("J'ai trouvé l'indice du channel %s : %d \n", salons[i]->nom, i);
    int j = 0;
    //On envoie le message à tous les clients connectés au channel
    while(j < getCount(salons[i])){
        if(salons[i]->clients[j] != socketClient)
        {
            printf("Le %d er client a qui il faut envoyer : %d \n", j,getClients(salons[i])[j]);
            switch(write(salons[i]->clients[j], messageEnvoi, strlen(messageEnvoi)* sizeof(char)))
            {
                case -1: 
                    perror("[-]Probleme a l'envoi du message");
                    closeAllsockets(utilisateursConnectes);
                    exit(-6);
                case 0:
                    fprintf(stderr, "[!]La socket a été fermée par le client!\n\n");
                    closeAllsockets(utilisateursConnectes);
                default:
                    printf("Message %s envoyé avec succés (%d octets)\n\n",Message,ecrits);
            }
        }
        j++;
    }

    /*
    Element * actuel = utilisateursConnectes -> premier;
    while(actuel -> suivant != NULL && liste_taille(utilisateursConnectes) > 0)
    {
        if(socketClient != actuel -> id)
        {
            ecrits = write(actuel -> id, messageEnvoi,strlen(messageEnvoi));
            switch(ecrits)
            {
                case -1: 
                    perror("[-]Problem of send the message");
                    closeAllsockets(utilisateursConnectes);
                    exit(-6);
                case 0:
                    fprintf(stderr, "[!]The socket was closed by the client !\n\n");
                    closeAllsockets(utilisateursConnectes);
                default:
                    printf("Message %s envoyé avec succés (%d octets)\n\n",Message,ecrits);
            }
        } 
        actuel = actuel->suivant;
    }
    */
}


/**
 * @brief 
 * fonction qui renvoie une réponse à un client lors de la connexion ou quand il demande la liste des commandes
 * @param socketClient socket du client à qui on doit envoyer ce message
 * @param Message Message à envoyer
 */
void reponseClient(int socketClient, char * Message)
{
    int ecrits;
    ecrits = write(socketClient, Message,strlen(Message));
    switch(ecrits)
    {
        case -1: 
            perror("[-]Problem of send the message");
            closeAllsockets(utilisateursConnectes);
            exit(-6);
        case 0:
            fprintf(stderr, "[!]The socket was closed by the client !\n\n");
            closeAllsockets(utilisateursConnectes);
        default:
            printf("Message %s envoyé avec succés (%d octets)\n\n",Message,ecrits);
    }
} 
 




/**
 * @brief 
 * fonction qui vérifie si un pseudo est disponible, donc qu'aucun client ne l'a choisit 
 * @param pseudo pseudo à chercher
 * @return int Renvoie 1 si ce pseudo existe, sinon 0
 */
int existPseudo(char * pseudo) {
    int result = 0;
    Element * actuel = utilisateursConnectes -> premier;
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


/**
 * @brief 
 * Récupération des informations sur un channel en particulier
 * @param socket Socket du client qui demande ces informations
 */
void infosChannels(int socket){
    char data[2048];
    memset(data, 0, 2048*sizeof(char));
    int i = 0;
    strcat(data, "Les informations du salon sont : \n");
    while(i<sizeofArraySalons && salons[i] != NULL){
        strcat(data, "-> Le salon ");
        strcat(data, salons[i]->nom);
        strcat(data," qui a pour description : ");
        strcat(data, salons[i]->description);
        strcat(data,". Elle contient actuellement ");
        char countChar[5];
        sprintf(countChar, "%d", salons[i]->count);
        strcat(data, countChar);
        strcat(data," clients avec une limite de ");
        char capacityChar[5];
        sprintf(capacityChar, "%d", salons[i]->capacity);
        strcat(data, capacityChar);
        strcat(data," clients \n\n");
        i++;
    }
    
    if(i==0){
        strcat(data, "Malheuresement il n'y a pas de channels existants\n");
    }

    switch(write(socket, data, 2048*sizeof(char))){
        case -1:
            perror("[-]Erreur rencontrée dans l'envoi des infos du channel");
            exit(-1);
        case 0:
            perror("La socket a été fermée par le client");
            exit(-1);
    }
}


/**
 * @brief 
 * Renvoie au clients une liste des différents channels existants
 * @param socket Socket du client qui demande la liste des cannals
 */
void listeChannels(int socket){
    char data[1024];
    memset(data, 0, 1024*sizeof(char));
    strcat(data, "La liste des channels est :\n");
    int i = 0;
    while(salons[i] != NULL){
        char indice[3];
        sprintf(indice, "%d", i);
        strcat(data, indice);
        strcat(data, " -> ");
        strcat(data, salons[i]->nom);
        strcat(data, "\n");
        i++;
    }
    switch(write(socket, data, 1024* sizeof(char))){
        case -1 :
            perror("Une erreur s'est produite dans l'envoi du contenu au client");
            exit(-1);
        case 0 : 
            perror("La socket a été fermée par le client");
            exit(-1);
    }
}

/**
 * @brief 
 * 
 */
int * createNewSocket()
{
    int socketServeur2;
    struct sockaddr_in pointDeRencontreLocal;
    socklen_t longueurAdresse;

    int socketClient2;
    struct sockaddr_in pointDeRencontreDistant;
    int retour;

    // Création d'une socket de communication pour le serveur
    // PF_INET c'est le domaine pour le protocole internet IPV4 
    socketServeur2 = socket(PF_INET, SOCK_STREAM, 0);
    // 0 indique que l’on utilisera le protocole par défaut associé à SOCK_STREAM soit TCP
    

    // Teste la valeur renvoyée par l’appel système socket()
    // afin de vérifier la bonne création de celle-ci
    if(socketServeur2 < 0)
    {
        perror("[-]Echec dans la création de la socket\n");
        exit(-1); // On sort en indiquant un code erreur
    }
    printf("[+]La socket a été créée avec succés ! (%d)\n", socketServeur2);
    
    longueurAdresse = sizeof(pointDeRencontreLocal);

    // Initialise à 0 la struct sockaddr_in
    memset(&pointDeRencontreLocal, 0x00, longueurAdresse);

    // Renseigne la structure sockaddr_in avec les informations du serveur distant
    pointDeRencontreLocal.sin_family = PF_INET;
    // Toutes les interfaces
    pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //on incrémente la valeur de la variable port pour éviter d'avoir une connexion sur un même port
    pthread_mutex_lock(&mutexPort);
    port++;
    pthread_mutex_unlock(&mutexPort);

    pointDeRencontreLocal.sin_port =  htons(port);
    inet_aton(ip,&pointDeRencontreLocal.sin_addr);
    
    // On demande l'attachement local de la socket 
    if((bind(socketServeur2,(struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
    {
        perror("[-]Probleme rencontré avec le binding");
        exit(-2);
    }

    printf("[+]Socket attachée avec succés !\n");

    // maximum 10 clients dans la fille
    if(listen(socketServeur2, 10) < 0)
    {
        perror("[-]Le serveur ne peut pas se mettre à l'écoute\n");
        exit(-3);
    }
    
    printf("[+]Serveur En Ecoute! \n");
    //Boucle d'attente de connexion: en théorie, un serveur attend indéfiniment
    //Dans un premier temps, il faut s'assurer qu'on a bien deux clients qui vont se connecter
    //on va d'abord rester dans cette boucle, tant que deux clients ne se sont pas bien connectés
    printf("Attente d'une demande de connexion (quitter avec Ctrl-C) \n\n");
    // c'est un appel bloquant 
    socketClient2 = accept(socketServeur2, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
    if(socketClient2 < 0)
    {
        perror("[-]Nous n'arrivons pas à connecter le client\n");
        close(socketClient2);
        close(socketServeur2);
        exit(-4);
    }
    int * tabSockets = malloc(2*sizeof(int));
    tabSockets[0]  = socketClient2;
    tabSockets[1]  = socketServeur2;
    printf("On a reussi à connecter le client \n");
    return tabSockets;
}



/**
 * @brief 
 * thread qui s'occupe de la réception d'un fichier envoyé par un client
 */
void * receptionFichier(){
    
    int * tabSockets = malloc(2*sizeof(int));
    tabSockets = createNewSocket();
        
    int socketClient2 = tabSockets[0];
    int socketServeur2 = tabSockets[1];

    //1) récupération du nom de fichier
    char * name = malloc(50*sizeof(char));
    int readName = read(socketClient2, name, 50*sizeof(char));
    if(readName == -1){
        perror("[-] Erreur dans la reception du nom de fichier\n");
        exit(-1);
    }
    printf("Le nom du fichier a recevoir est : %s\n", name);

    //2) récupération de la taille du fichier qu'on va récupérer
    char * tailleChar = malloc(10*sizeof(char));
    
    int readTaille = read(socketClient2,tailleChar, 10*sizeof(char)); 
    if( readTaille == -1){
        perror("[-] Une erreur est survenu, nous n'arrivons pas à recevoir la taille du fichier que vous voulez envoyer \n");
        exit(-1);
    }
    
    int taille = atoi(tailleChar);
    printf("La taille du message est : %d\n", taille);

    //3) Récupération du contenu du fichier que l'on va créer
    char * data = malloc(taille*sizeof(char)); 
    int readData = read(socketClient2,data, taille * sizeof(char));
    if( readData == -1){
        perror("[-] Une erreur est survenu, nous n'arrivons pas à recevoir le contenu du fichier\n");
        exit(-1);
    }
    
    printf("Voila le message recu = %s\n", data);

    //4) Création du fichier dans le dossier souhaité
    char path[150] = "fichiersServeur/";
    strcat(path, name);

    FILE * f = fopen(path, "w");

    if(f != NULL){
        fputs(data, f);
        fclose(f);
    } 
    // fin de l'ajout
    close(socketServeur2);
    close(socketClient2);
    pthread_exit(0);
}


/**
 * @brief 
 * Crée un nouveau channel et l'ajoute au tableau de channels
 * @param socket 
 */
void * createCanal(){
    int * tabSockets = malloc(2*sizeof(int));
    tabSockets = createNewSocket();
        
    int socket = tabSockets[0];
    int socketServeur2 = tabSockets[1];
    // 1) On vérifie si il reste de la place dans le serveur pour un nouveau channel
    if(nombreSalons >= sizeofArraySalons){
        switch(write(socket, "impossible", 100 * sizeof(char))){
            case -1 :
                perror("[-] Problème dans l'envoi du diagnostic");
                exit(-1);
            case 0 : 
                perror("[-] La socket a été fermée par le client");
                exit(-1);
        }
        printf("[!] Il ne reste plus de places pour un nouveau channel\n");
        close(socketServeur2);
        close(socket);
        pthread_exit(0);
    }
    else{
        switch(write(socket, "possible", 100 * sizeof(char))){
            case -1 :
                perror("[-] Problème dans l'envoi du diagnostic");
                exit(-1);
            case 0 : 
                perror("[-] La socket a été fermée par le client");
                exit(-1);
        }
    }

    // 2) Réception du nom du canal
    char * name = malloc(100 * sizeof(char));
    switch(read(socket, name, 100 * sizeof(char))){
        case -1 :
            perror("[-] Problème dans la reception du nom du nouveau channel");
            exit(-1);
        case 0 : 
            perror("[-] La socket a été fermée par le client");
            exit(-1);
    }
    printf("Voici le nom du channel : %s\n", name);

    // 3) Réception de la description du salon
    char * description = malloc(1024 * sizeof(char));
    switch(read(socket, description, 1024 * sizeof(char))){
        case -1 :
            perror("[-] Problème dans la reception de la description du nouveau channel");
            exit(-1);
        case 0 : 
            perror("[-] La socket a été fermée par le client");
            exit(-1);
    }
    printf("Voici la description du nouveau channel : %s\n", description);

    // 4) Réception de la taille maximum du salon
    char * tailleMax = malloc(5*sizeof(char));
    switch(read(socket, tailleMax, 5 * sizeof(char))){
        case -1 :
            perror("[-] Problème dans la reception de la description du nouveau channel");
            exit(-1);
        case 0 : 
            perror("[-] La socket a été fermée par le client");
            exit(-1);
    }
    int taille = atoi(tailleMax);
    printf("La taille max est %d\n", taille);
    salons[nombreSalons] = cree_Channel(name, description, taille);
    nombreSalons += 1;

    // fin de la creation du canal
    close(socketServeur2);
    close(socket);
    pthread_exit(0);
}




/**
 * @brief 
 * Fonction qui permet de changer de channel
 * @param socket socket du client
 * @param canal Nom du channel sur lequel on sohaite se connecter
 */
void changementCanal(void * SocketClient, char * canal){
    int socket = (long) SocketClient;
    int nonExist = 1;
    int i = 0;
    char data[1024]; // reponse a envoyé au client
    memset(data, 0, 1024*sizeof(char));
    while(i < nombreSalons && nonExist)
    {
        //on cherche d'abord parmi tous les salons si un d'entre eux porte ce nom
        if(strcmp(salons[i]->nom, canal) == 0)
        {
            //On a trouvé le channel vers lequel le client souhaite se connecter, on ajoute donc le client a ce channel 
            nonExist = 0;
            int j = 0;
            int nonFini = 1;
            //On supprime le client du channel auquel il est connecté avant
            while(j < nombreSalons && nonFini)
            {
                if(strcmp(salons[j]->nom, getCanalClient(utilisateursConnectes, socket)) == 0){
                    supprimer_client(salons[j], socket);
                    nonFini = 0;
                }
                j++;
            }

            ajouter_client(salons[i], socket); 
            afficheClients(salons[i]);
            if(nonFini)
            {
                nonFini = 0;
                printf("Le client n'appartenait à aucun canal\n");
            }

            if(modifierCanalClient(utilisateursConnectes, socket, canal) == -1){
                perror("erreur dans la modification du channel du client \n");
            }
        }
        i++;
    } 
    if(nonExist){
        strcat(data, "-!---Le canal que vous avez saisi n'existe pas---!-\n\n");
    }
    else{
        strcat(data, " [+] Vous avez réussi à changer de canal en passant au canal : ");
        strcat(data, getCanalClient(utilisateursConnectes, socket));
        strcat(data, "\n");
    }
    switch(write(socket, data, 1024 * sizeof(char))){
        case -1 :
            perror("[-] Problème dans l'envoi des infos pour le changement de canal");
            exit(-1);
        case 0 : 
            perror("[-] La socket a été fermée par le client");
            exit(-1);
    }
}


/**
 * @brief   
 * Lister les fichiers disponible dans un dossier
 * et retourner le nombre
 * @param dossier dossier dans lequel se trouve les fichiers du serveur
 * @return int fichier nombre de fichiers disponible
 */
void listesFichierDansDos(char * dossier, int socket)
{   
    char * data = malloc(longueurMessage*sizeof(char));
    memset(data, 0, longueurMessage*sizeof(char));
    struct dirent *dir;
    int nbFichiers = 0;
    // opendir() renvoie un pointeur de type DIR. 
    DIR * d = opendir(dossier); 
    if (d)
    {
        strcat(data,"\n\n Liste des fichiers disponibles côté serveur : \n\n");
        while ((dir = readdir(d)) != NULL)
        {
            printf("-> %s\n",dir->d_name);
            strcat(data,"-> ");
            strcat(data, dir->d_name);
            strcat(data, "\n");
            nbFichiers++;
        }
        closedir(d);
        switch (write(socket,data, strlen(data) * sizeof(char))){
            case -1: 
                perror("[-] Problème dans l'envoi de la liste des fichiers présents côté serveur\n");
                exit(-1);
            case 0:
                perror("[-] La connexion a été fermée par le client \n");
                exit(-1);
            default:
                printf("La liste des fichiers existants a bien été envoyé \n");
        }
    }
}

/**
 * @brief 
 * Renvoie la liste des différents clients connectées
 * @param socket 
 */
void listeUsers(int socket){
    char data[1024];
    memset(data, 0, 1024*sizeof(char));
    int taille = liste_taille(utilisateursConnectes);
    int i;
    strcat(data, " La liste des utilisateurs est : \n");
    Element * e = utilisateursConnectes->premier;
    for(i = 0; i< taille; i++)
    {
        strcat(data, " -> ");
        strcat(data, e->pseudo);
        strcat(data, " (id : ");
        char id[3];
        sprintf(id, "%d", e->id);
        strcat(data, id);
        strcat(data, ") ");
        strcat(data,"et connecté au canal : ");
        strcat(data, e->canal);
        strcat(data, "\n");
        e = e->suivant;
    }
    switch(write(socket, data, 1024* sizeof(char))){
        case -1 :
            perror("Une erreur s'est produite dans l'envoi du contenu au client");
            exit(-1);
        case 0 : 
            perror("La socket a été fermée par le client");
            exit(-1);
    }
}




/**
 * @brief 
 * thread qui va s'occuper en problème du traitement reçu de la part du client pour l'acheminer vers la bonne fonction, 
 * il va être lancé pour chaque nouveau client qui se connecte
 * @param SocketClient Socket (id du client) pour laquelle on va lancer ce thread
 * @return void* 
 */
void * Relayer(void * SocketClient)
{   
    int socketClient = (long) SocketClient;
    //Ecrasement les données contenues dans messageEnvoi et messageRecu pour éviter les données non désirables
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
                closeAllsockets(utilisateursConnectes);
                exit(-5);
            case 0:
                fprintf(stderr, "[!]La socket a été fermé par le client !\n\n");
                closeAllsockets(utilisateursConnectes);
            default:
                if(i < 1)
                {        
                    // Connexion du client avec l'entrée d'un pseudo valide             
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
                            char * canal = malloc(100* sizeof(char));
                            strcpy(canal, getName(salons[0]));
                            printf("on ajoute le pseudo : %s au canal %s\n",pseudo, canal);
                            ajouter_debut(utilisateursConnectes,socketClient, pseudo, canal);
                            printf("taille apres l'ajout : %d \n",liste_taille(utilisateursConnectes));
                            afficherListe(utilisateursConnectes);  
                            //une fois qu'on a réussi à connecter le client, on lance le thread qui va relayer les messages
                            nombreClientsConnectes = liste_taille(utilisateursConnectes);              
                            // chaque client à son propre identifiant 
                            ajouter_client(salons[0], socketClient);
                            printf("Le nombre de clients connectés a l'accueil est %d \n",salons[0]->count);
                            afficheClients(salons[0]);
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
            // Après s'être bien connecté, le client peut envoyer des messages
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
                int i;
                for(i = 0; i < nombreSalons; i++)
                {
                    if(strcmp(getCanalClient(utilisateursConnectes, socketClient), getName(salons[i])) == 0 )
                    {
                        supprimer_client(salons[i], socketClient);
                    }
                }
                supprimer_val(utilisateursConnectes, socketClient);
                nombreClientsConnectes = liste_taille(utilisateursConnectes);
                printf("La taille actuelle après déconnexion est de : %d\n", liste_taille(utilisateursConnectes));
                afficherListe(utilisateursConnectes);
                close(socketClient);
                pthread_exit(0);
            }
            else if(strcmp(separation[0],"@") == 0)
            {
                printf("on est dans @ case \n");
                strcpy(messageEnvoi, separation[2]);    
                printf("le message d'envoie est %s \n", messageEnvoi);
                pthread_mutex_lock(&mutex);
                EnvoyerMessageSpe(socketClient, messageEnvoi, separation[1], separation[0]);        
                pthread_mutex_unlock(&mutex);
            }
            else if(strcmp(separation[0],"/mp") == 0)
            {
                printf("on est dans le message privé case \n");
                strcpy(messageEnvoi, separation[2]);    
                pthread_mutex_lock(&mutex);
                    EnvoyerMessageSpe(socketClient, messageEnvoi, separation[1], separation[0]);        
                pthread_mutex_unlock(&mutex);
            }
            else if(strcmp(separation[0],"!") == 0)
            {
                /* Urgent */
                printf("on est dans ! case \n");
                strcpy(messageEnvoi, separation[2]);    
                pthread_mutex_lock(&mutex);
                    EnvoyerMessageSpe(socketClient, messageEnvoi, separation[1], separation[0]);        
                pthread_mutex_unlock(&mutex);
            }
            else if(strcmp(separation[0],"/help") == 0)
            {
                printf("on est dans /help case \n");
                strcpy(messageEnvoi, Commandes());    
                reponseClient(socketClient, Commandes());     
            }
            else if(strcmp(separation[0],"file") == 0)
            {
                printf("Nous avons reçu l'information que tu veux envoyer un fichier\n");
                pthread_t tFiles;
                pthread_create(&tFiles, NULL, receptionFichier,NULL);
                printf("fichier recu et enregistrer avec succés ! \n");
            }
            else if(strcmp(separation[0],"/files") == 0){
                printf("On va procéder à l'envoi du contenu du dossier sur le serveur\n");
                listesFichierDansDos("fichiersServeur", socketClient);
            }
            else if(strcmp(separation[0],"/channels") == 0){
                printf("Nous allons envoyer la liste des channels au client\n");
                listeChannels(socketClient);
            }
            else if(strcmp(separation[0],"/users") == 0){
                printf("On va vous envoyer la liste des utilisateurs connectés...\n");
                listeUsers(socketClient);
            }
            else if(strcmp(separation[0],"join") == 0){
                printf("On va essayer de joindre un nouveau channel\n");
                changementCanal(SocketClient, separation[1]);       
            }
            else if(strcmp(separation[0],"createChannel") == 0){
                printf("On va procéder a la creation du channel\n");
                pthread_t tCanal;
                pthread_create(&tCanal, NULL, createCanal, NULL);
                pthread_join(tCanal, NULL);
            }
            else if(strcmp(separation[0],"infoChannels") == 0) {
                printf("On va vous envoyer les informations des channels...\n\n");
                infosChannels(socketClient);
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
    closeAllsockets(utilisateursConnectes);
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

    utilisateursConnectes = cree_liste();
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

    port = atoi(argv[2]);
    
    pointDeRencontreLocal.sin_port =  htons(port);
    ip = malloc(50*sizeof(char));
    strcpy(ip, argv[1]);
    inet_aton(ip,&pointDeRencontreLocal.sin_addr);
    
    // On demande l'attachement local de la socket 
    if((bind(socketServeur,(struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
    {
        perror("[-]Problem of binding");
        exit(-2);
    }

    printf("[+]Socket attached with success!\n");

    //Création du salon d'accueil et initialisations à NULL du tableau de salons
    memset(salons, 0, sizeofArraySalons * sizeof(Channel *));
    salons[0] = cree_Channel("Accueil", "Salon principal d'accueil des nouveaux clients", 20);
    salons[1] = cree_Channel("IG", "Channel spécial pour les etudiants en IG", 25);
    salons[2] = cree_Channel("MAT", "Channel spécial pour les Matériaux", 10);
    nombreSalons = 3;


    // maximum 10 clients dans la fille
    if(listen(socketServeur, 10) < 0)
    {
        perror("[-]The server can not listen");
        exit(-3);
    }
    
    while(1){
        printf("[+]Serveur En Ecoute! \n");
        printf("nombre de clients connectés %d\n", liste_taille(utilisateursConnectes));  
        //Boucle d'attente de connexion: en théorie, un serveur attend indéfiniment
        //Dans un premier temps, il faut s'assurer qu'on a bien deux clients qui vont se connecter
        //on va d'abord rester dans cette boucle, tant que deux clients ne se sont pas bien connectés
        printf("Attente d'une demande de connexion (quitter avec Ctrl-C) \n\n");
        // c'est un appel bloquant 
        socketDialogue = accept(socketServeur, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
        if(socketDialogue < 0)
        {
            perror("[-]Nous n'arrivons pas à connecter le client\n");
            close(socketDialogue);
            close(socketServeur);
            exit(-4);
        }
  
        //une fois qu'on a réussi à connecter le client, on lance le thread qui va traiter les messages reçus de sa part
        pthread_create(&tRelay, NULL, Relayer, (void *)(long) socketDialogue);
    }    
    close(socketServeur);
    return 0;
}