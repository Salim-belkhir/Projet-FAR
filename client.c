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
#include <time.h>
#include "client.h"


/* Definition des couleurs pour améliorer le visuel */
#define BLANC     "\033[37;01m"
#define MAUVE     "\033[35;01m"
#define BLEU      "\033[34;01m"
#define JAUNE     "\033[33;01m"
#define VERT      "\033[32;01m"
#define ROUGE     "\033[31;01m"
#define GRIS      "\033[30;01m"
#define BLEU_CIEL "\033[36;01m"
#define END       "\033[00m"

// la longueur maximale de message 
#define longueurMessage 10000


/******* Les variables globales ***********/ 

// Connecté ou pas!
int status = 0;

char * ip; //adresse IP du serveur à laquelle se connecte le client 

int port;  //numéro de port à lequelle il va se connecter

/******************************************/


/**
 * @brief 
 * liste les commandes pour les différents modes de message possibles
 */
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


/**
 * @brief 
 * fonction pour pouvoir séparer une chaîne de charactères en plusieurs parties
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
 * fonction qui permet la connexion du client au début, avec la sélection du pseudo
 * @param socketClient socket du client qui permet la communication avec le serveur
 */
void connection(int socketClient)
{
    int ecrits;  //stocker le retour de la fonction "write" pour savoir si c'est correct
    int lus;    //stocker le retour de la fonction "read" pour savoir si la lecture se déroule correctement
    char messageRecu[longueurMessage];  //où on stocke le message reçu par le serveur
    char messageEnvoi[longueurMessage];  //où on stocke le message à envoyer
    int socket = (long)socketClient; 
    
    int i = 0;
    while(status == 0)
    {   
        //Début de la connexion avec l'envoi du pseudo choisi
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        if( i < 1)
        {   
            do
            {        
                printf("%s▬▬ Voulez-vous vous connecter à votre compte ? %s(o/n) ▬▬\n",MAUVE,BLEU_CIEL);
                printf("→%s ",BLANC);
                fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
                messageEnvoi[strlen(messageEnvoi) - 1]= 0;
                //strcpy(messageEnvoi, "tentative de connexion");
            }while(strcmp(messageEnvoi,"o") != 0  && strcmp(messageEnvoi,"n") != 0);
        } else {            
            fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
            messageEnvoi[strlen(messageEnvoi) - 1]= 0;
        }
        ecrits = write(socket, messageEnvoi,strlen(messageEnvoi));
        switch(ecrits)
        {
            case -1:
                printf("%s",ROUGE); 
                perror("[-] la fonction Write a échoué");
                printf("%s\n",END);
                close(socket);
                exit(-3);
            case 0:
                printf("%s",ROUGE);
                printf("[-] La socket a été fermée par le serveur !\n");
                printf("%s\n",END);
                close(socket);
                exit(-5);
            default:
                if( i < 1)
                {
                    if(strcmp(messageEnvoi, "o") == 0){
                        printf("%s▬▬ Quels sont vos identifiants ? %s'pseudo mdp' ▬▬\n",MAUVE,BLEU_CIEL);
                        printf("→%s ",BLANC);
                    }
                    else{
                        printf("%s▬▬ Quel pseudo et mot de passe choisissez-vous ? %s'pseudo mdp' ▬▬\n",MAUVE,BLEU_CIEL);
                        printf("→%s ",BLANC);
                    }
                    fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
                        messageEnvoi[strlen(messageEnvoi) - 1]= 0;
                        switch(write(socket, messageEnvoi, longueurMessage * sizeof(char))){
                            case -1 :
                                printf("%s",ROUGE);
                                perror("[-] Erreur dans l'envoi des identifiants");
                                printf("%s\n",END);
                                exit(-1);
                            case 0 :
                                printf("%s",ROUGE);
                                perror("La socket a été fermée par le serveur");
                                printf("%s\n",END);
                                exit(-1);
                        }
                    
                    printf("%s▬▬ Demande de connexion %s%s. . .\n", MAUVE,END,BLEU_CIEL);
                }
                else
                {
                    printf("%s○ Pseudo %s envoyé avec %ssuccés ☻ (%d octets) %s○\n",MAUVE,messageEnvoi,BLEU_CIEL,ecrits,MAUVE);
                }                         
        }

        memset(messageRecu, 0x00, longueurMessage*sizeof(char));

        //on récupére la réponse du serveur pour savoir si le pseudo est bien disponible
        lus = read(socket, messageRecu, longueurMessage*sizeof(char)); 
        switch(lus)
        {
            case -1:
                printf("%s",ROUGE);
                perror("[-] La fonction read a échoué");
                printf("%s\n",END);
                close(socket);
                exit(-4);
            case 0:
                printf("%s",ROUGE);
                printf("[-] La socket a été fermée par le serveur !\n");
                printf("%s\n",END);
                close(socket);
                exit(-5);
            default:
                if(status == 0)
                {
                    if(strcmp(messageRecu, "valide") == 0)
                    {
                        status = 1;
                        printf("%s○ Le pseudo choisit est bien %svalide ☻ %s○\n",MAUVE,BLEU_CIEL,MAUVE);
                        printf("\n%s◘ CONNECTE ◘ \n",VERT);
                        sleep(1.5);
                        system("clear");
                        break;
                    }else if(strcmp(messageRecu, "invalide") == 0)
                    {
                        printf("%s○ Le pseudo choisit %sexist déja !! %s○\n",MAUVE,JAUNE,MAUVE);   
                    }
                    else if(strcmp(messageRecu, "Mot de passe invalide") == 0){
                        printf("%s[!] Le mot de passe est erroné\n",ROUGE);
                    }
                    else if(strcmp(messageRecu, "Votre identifiant est erroné") == 0){
                        printf("%s[!] Votre identifiant n'existe pas\n",ROUGE);
                    }
                    else if(strcmp(messageRecu, "Votre etes déja connecté sur une autre session!") == 0){
                        printf("%s[!] Votre etes déja connecté sur une autre session!\n",ROUGE);
                    }
                    else {
                        i++;
                        printf("▬▬ %s%s ▬▬ \n",MAUVE,messageRecu);
                        printf("%s→ %s", BLEU_CIEL, BLANC);
                    }
                } 
        }
    }    
}


/**
 * @brief   
 * Lister les fichiers disponible dans un dossier
 * et retourner le nombre
 * @param dossier dossier qui contient les fichiers côté client
 * @return int Nombre de fichiers disponible
 */
int listeFichierDansDos(char * dossier, char ** fichiers)
{
    struct dirent *dir;
    int nbFichiers = 0;
    // opendir() renvoie un pointeur de type DIR. 
    DIR * d = opendir(dossier); 
    if (d)
    {
        printf("%s▬▬ Listes des fichiers possibles d'envoyer ▬▬\n",MAUVE);
        while ((dir = readdir(d)) != NULL)
        {
            fichiers[nbFichiers] = malloc(256*sizeof(char)); 
            fichiers[nbFichiers] = dir->d_name;
            if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                printf("%s->%s %s\n",BLEU_CIEL,BLANC,dir->d_name);
            }
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

/**
 * @brief Creation d'une nouvelle Socket 
 * 
 * @return int revoie la socket créer
 */

int createNewSocket()
{
    // On va créer une nouvelle connexion avec le serveur sur un nouveau port pour 
    // l'envoi du fichier sans interferer avec l'envoi et la réception de messages 
    // OU pour d'autre utilité comme les channels

    long socketClient;
    struct sockaddr_in pointDeRencontreDistant;
    socklen_t longueurAdresse;

    socketClient = socket(PF_INET, SOCK_STREAM, 0);
    /* 0 indique que l’on utilisera leprotocole par défaut 
    associé à SOCK_STREAM soit TCP
    */

    // Teste la valeur renvoyée par l’appel système socket() afin de vérifier la bonne création de celle-ci
    if(socketClient < 0)
    {
        printf("%s",ROUGE);
        perror("[-] Creation socket echoué");
        printf("%s\n",END);
        exit(-1); // On sort en indiquant un code erreur
    }
    
    longueurAdresse = sizeof(pointDeRencontreDistant);
    // Initialise à 0 la struct sockaddr_in
    memset(&pointDeRencontreDistant, 0x00, longueurAdresse);

    // Renseigne la structure sockaddr_in avec les informations du serveur distant
    pointDeRencontreDistant.sin_family = PF_INET;
    port++;
    pointDeRencontreDistant.sin_port =  htons(port);
    inet_aton(ip,&pointDeRencontreDistant.sin_addr);

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
    return socketClient;
}


/**
 * @brief 
 * Thread qui s'occupe de l'envoi d'un fichier vers le serveur 
 * @param fileName Nom du fichier à envoyer 
 */
void  * envoiFile(void * fileName){

    char * filename = (char *) fileName;
    int socketClient = createNewSocket();  
    /*  La connexion est réussie, on peut donc commencer l'envoi du fichier  */

    // 1) Envoi du nom du fichier
    int writeName = write(socketClient, filename, 50*sizeof(char));
    switch(writeName){
        case -1 : 
            printf("%s",ROUGE);
            perror("[-] Problème rencontré dans l'envoi du fichier au serveur\n");
            printf("%s\n",END);
            exit(-1);
        case 0 :
            perror("[-] La socket a été fermée par le serveur");
            exit(-1);
        default :
            printf("\n%s○ Le nom du fichier %s%s%s a été envoyé ○\n",MAUVE,BLANC,filename,MAUVE);
    }

    char path[150] = "fichiersClient/";
    strcat(path, filename); 

    // 2) on recupere maintenant la taille du fichier et on l'envoie
    int taille = tailleFile(path);
    char* tailleChar =  malloc( 10 * sizeof(char));
    sprintf(tailleChar, "%d", taille);

    int writeTaille = write(socketClient, tailleChar, 10*sizeof(char));
         
    switch(writeTaille){
        case -1 :
            printf("%s",ROUGE);
            perror("[-] Problème dans l'envoi de la taille du fichier\n");
            printf("%s\n",END);
            exit(-1);
        case 0 :
            printf("%s",ROUGE);
            perror("[-] La connexion a été fermée par le serveur\n");
            printf("%s\n",END);
            exit(-1);
        default :
            printf("○ Taille du fichier envoyée avec succés ○\n");
    } 
    

    // 3) On ouvre le fichier pour en extraire le contenu et l'envoyer
    FILE * fp = fopen(path,"rb");
    
    if (fp == NULL) {
        printf("%s",ROUGE);
        perror("[-]Le fichier n'a pas pu être lu ou est introuvable ! \n");
        printf("%s\n",END);
        exit(1);
    }

    char data[taille];  //buffer qui va contenir le contenu du fichier

    fread(data, taille, 1, fp);
    fclose(fp);

    int writeData = write(socketClient, data, taille*sizeof(char));
    switch(writeData){
        case -1:
            printf("%s",ROUGE);
            perror("[-]Erreur rencontrée dans l'envoi du contenu du fichier");
            printf("%s\n",END);
            exit(-1);
        case 0:
            printf("%s",ROUGE);
            perror("[-]La socket a été fermée par le serveur");
            printf("%s\n",END);
            exit(-1);
        default :
            printf("○ Contenu du fichier bien envoyé ○\n");
            printf("%s☼☼☼ Envoyer Un Message ☼☼☼%s\n", BLEU_CIEL,BLANC);
    }
    close(socketClient);  //fermeture de cette connexion
    pthread_exit(0);
}

/**
 * @brief 
 * fonction qui permet le bon choix du fichier à envoyer et ensuite crée un thread qui s'occupe de l'envoi
 * @param socket socket du Client pour pouvoir envoyer "file" au serveur une fois qu'un fichier valide a été sélectionné
 */
void procFichier(int socket)
{
    char ** fichiers = malloc(256*sizeof(char));
    FILE *fp;
    char *nomFichier = malloc(longueurMessage*sizeof(char));
    char *fichierChoisit = malloc(longueurMessage*sizeof(char));
    pthread_t tFiles;

    char *dossier = malloc(longueurMessage*sizeof(char));
    int i , nombreFichiers, fichiersTrouver;
    
    int writeData = write(socket, "/file",strlen("/file"));
    switch(writeData){
        case -1:
            printf("%s",ROUGE);
            perror("[-]Erreur rencontrée dans l'envoi message file");
            printf("%s\n",END);
            exit(-1);
        case 0:
            printf("%s",ROUGE);
            perror("[-] La socket a été fermée par le serveur");
            printf("%s\n",END);
            exit(-1);
        default :
            printf("%s○ le message %s/file %s a été bien envoyer ○\n",MAUVE, BLEU_CIEL ,MAUVE);
    }
    
    strcpy(dossier, "fichiersClient/");   
    nombreFichiers = listeFichierDansDos(dossier, fichiers);
    fichiersTrouver = 0;
    while(fichiersTrouver == 0){
        printf("%s", MAUVE);
        puts("☼☼☼ Choisissez un fichier ☼☼☼");
        printf("→%s ",BLANC);
        fgets(nomFichier, longueurMessage*sizeof(char),stdin);
        nomFichier[strlen(nomFichier) - 1]=0;
        for(i=0;i<nombreFichiers;i++)
        {     
            if(strcmp(nomFichier,fichiers[i]) == 0)
            {
                printf("○ Le fichier existe bien ! ○\n");
                fichiersTrouver = 1;
                printf("○ le fichier a été lu correctement ○\n");
                strcpy(fichierChoisit, nomFichier);
                pthread_create(&tFiles, NULL, envoiFile,(void *) fichierChoisit);
                printf("○ [+]Fichier a été bien envoyer ○\n");
            }
        }
        if(fichiersTrouver == 0)
        {
            printf("○ Ce fichier n'existe pas ○\n");
        }
    }
}


/**
 * @brief 
 * thread qui s'occupe de la réception d'un fichier envoyé par un client
 */
void receptionFichier(int socketClient2){          
    //1) récupération du nom de fichier
    char * name = malloc(50*sizeof(char));
    int readName = read(socketClient2, name, 50*sizeof(char));
    if(readName == -1){
        printf("%s",ROUGE);
        perror("[-] Erreur dans la reception du nom de fichier\n");
        printf("%s\n",END);
        exit(-1);
    }
    printf("○ Le nom du fichier a recevoir est : %s ○\n", name);

    //2) récupération de la taille du fichier qu'on va récupérer
    char * tailleChar = malloc(10*sizeof(char));
    
    int readTaille = read(socketClient2,tailleChar, 10*sizeof(char)); 
    if( readTaille == -1){
        printf("%s",ROUGE);
        perror("[-] Une erreur est survenu, nous n'arrivons pas à recevoir la taille du fichier que vous voulez envoyer \n");
        printf("%s\n",END);
        exit(-1);
    }
    
    int taille = atoi(tailleChar);
    printf("○ La taille du message est : %d ○\n", taille);

    //3) Récupération du contenu du fichier que l'on va créer
    char * data = malloc(taille*sizeof(char)); 
    int readData = read(socketClient2,data, taille * sizeof(char));
    if( readData == -1){
        printf("%s",ROUGE);
        perror("[-] Une erreur est survenu, nous n'arrivons pas à recevoir le contenu du fichier\n");
        printf("%s\n",END);
        exit(-1);
    }
    
    printf("○ Voila le message recu = %s ○\n", data);

    //4) Création du fichier dans le dossier souhaité
    char path[150] = "fichiersClient/";
    strcat(path, name);

    FILE * f = fopen(path, "wb");

    if(f != NULL){
        fputs(data, f);
        fclose(f);
    } 
}


/**
 * @brief fonction qui permet au client de modifier les informations d'un channel au client 
 * La modification se fait sur une nouvelle socket pour permettre le client de continuer de recevoir des messages et ne pas 
 * avoir de conflit
 */
void modifierCanal(){
    int socket = createNewSocket();

    // (1) Modification du nom du canal
    char * data = malloc(1024 * sizeof(char));
    switch(read(socket,data, 1024 * sizeof(char))){
        case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème rencontré dans la réception d'informations de la part du serveur \n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("[-] La socket a été fermée par le client \n");
            printf("%s\n",END);
            return;
    }
    if(strcmp(data, "erreur") == 0){
        printf("%s",ROUGE);
        printf("[-] Ce channel n'existe pas");
        printf("%s\n",END);
        return;
    }
    printf("%s▬▬ le nom du canal à modifier %s%s %s▬▬",MAUVE,data,BLANC,MAUVE);
    printf("%s\n", data);
    printf("▬▬ Voulez-vous le modifier ? %s(o/n) ▬▬\n",BLEU_CIEL);
    printf("→%s ", BLANC);
    memset(data, 0x00, 1024*sizeof(char));
    fgets(data, 1024*sizeof(char),stdin);
    data[strlen(data) - 1]=0;
    switch(write(socket, data, 1024* sizeof(char))){
        case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème dans l'envoi des informations au serveur \n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("[-] La socket a été fermée par le serveur");
            printf("%s\n",END);
            return;
    }
        // Volonté de modifier le nom
    if(strcmp(data, "o") == 0){
        printf("%s▬▬ Quel est le nouveau nom que vous voulez donner ? ▬▬%s\n",MAUVE,BLEU_CIEL);
        printf("→%s ", BLANC);
        memset(data, 0x00, 1024*sizeof(char));
        fgets(data, 1024*sizeof(char),stdin);
        data[strlen(data) - 1]=0;
        switch(write(socket,data, 1024 * sizeof(char))){
            case -1 :
                printf("%s",ROUGE);
                printf("[-] Problème dans l'envoi des informations au serveur [-] \n\n");
                printf("%s\n",END);
                close(socket);
                return;
            case 0 :
                printf("%s",ROUGE);
                printf("[-] La socket a été fermée par le serveur \n");
                printf("%s\n",END);
                return;
        }
    }

    memset(data, 0x00, 1024*sizeof(char));

    // (2) Modification de la description
    printf("%s○ Voici la description du Canal actuelle ci-dessous ○\n",MAUVE);
    
    //récupération de la description existante
    switch(read(socket, data, 1024 * sizeof(char))){
        case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème rencontré dans la réception d'informations de la part du serveur [-] \n\n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("La socket a été fermée par le client");
            printf("%s\n",END);
            return;
    }
    printf("%s%s\n",BLANC,data);
    printf("%s▬▬  Voulez-vous modifier la description ? %s(o/n) ▬▬\n",MAUVE,BLEU_CIEL);
    printf("→%s ", BLANC);
    memset(data, 0x00, 1024*sizeof(char));
    
    // Récupération de la réponse du client et envoi au serveur
    fgets(data, 1024*sizeof(char),stdin);
    data[strlen(data) - 1]=0;
    switch(write(socket,data, 1024 * sizeof(char))){
        case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème dans l'envoi des informations au serveur\n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("[-] La socket a été fermée par le serveur \n");
            printf("%s\n",END);
            return;
    }
        //envoi de la nouvelle description
    if(strcmp(data, "o") == 0){
        printf("%s▬▬ Quelle est la nouvelle description que vous voulez ajouter ? ▬▬%s\n",MAUVE,BLEU_CIEL);
        printf("→%s ", BLANC);
        memset(data, 0x00, 1024*sizeof(char));
        fgets(data, 1024*sizeof(char),stdin);
        data[strlen(data) - 1]=0;
        switch(write(socket,data, 1024 * sizeof(char))){
            case -1 :
                printf("%s",ROUGE);
                printf("[-] Problème dans l'envoi des informations au serveur \n");
                printf("%s\n",END);
                close(socket);
                return;
            case 0 :
                printf("%s",ROUGE);
                printf("[-] La socket a été fermée par le serveur \n");
                printf("%s\n",END);
                return;
        }
    }


    memset(data, 0x00, 1024*sizeof(char));

    // (3)  Modification de la capacité maximale
    switch(read(socket,data, 1024 * sizeof(char))){
        case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème rencontré dans la réception d'informations de la part du serveur \n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("La socket a été fermée par le client \n");
            printf("%s\n",END);
            return;
    }
     // récupération de la capacité actuelle
    printf("%s○ La taille actuelle est de %s%s %s○\n",MAUVE,data,BLEU_CIEL,MAUVE);
    printf("▬▬ Voulez vous modifier la capacité maximale du canal ? %s(o/n) ▬▬\n",BLEU_CIEL);
    printf("→%s ", BLANC);
    memset(data, 0x00, 1024*sizeof(char));
    fgets(data, 1024*sizeof(char),stdin);
    data[strlen(data) - 1]=0;
    switch(write(socket,data, 1024 * sizeof(char))){
        case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème dans l'envoi des informations au serveur \n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("[-] La socket a été fermée par le serveur \n");
            printf("%s\n",END);
            return;
    }
        // modification de la capacité maximale
    if(strcmp(data, "o") == 0){
        printf("%s▬▬ Quelle est la nouvelle capacité maximale ? ▬▬%s\n",MAUVE,BLEU_CIEL);
        printf("→%s ", BLANC);
        memset(data, 0x00, 1024*sizeof(char));
        fgets(data, 1024*sizeof(char),stdin);
        data[strlen(data) - 1]=0;
        switch(write(socket,data, 1024 * sizeof(char))){
            case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème dans l'envoi des informations au serveur \n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("[-] La socket a été fermée par le serveur \n");
            return;
        }   
    }
    memset(data, 0x00, 1024*sizeof(char));
    switch(read(socket,data, 1024 * sizeof(char))){
            case -1 :
            printf("%s",ROUGE);
            printf("[-] Problème dans l'envoi des informations au serveur \n");
            printf("%s\n",END);
            close(socket);
            return;
        case 0 :
            printf("%s",ROUGE);
            printf("[-] La socket a été fermée par le serveur \n");
            return;
        default:
            printf("%s\n",data);   
    }
    close(socket);
}

/**
 * @brief Recevoir (Téléchargé ) un fichier disponible sur le serveur 
 * Un thread applique cette fonction qui permet le bon choix du fichier à recevoir et le recoie
 */
void * procRecupFichier()
{
    int socket = createNewSocket();
    int fichiersTrouver = 0;
    char * nomFichier = malloc(50*sizeof(char));
    char * messageRecu = malloc(longueurMessage*sizeof(char));

    while(fichiersTrouver == 0){
        
        if (read(socket,messageRecu, longueurMessage*sizeof(char)) == -1) {
        printf("%s",ROUGE);
        perror("[-]Erreur à la lecture du message recu pour les fichiers. ");
        printf("%s\n",END);
        exit(1);
        }
        
        //lister les fichiers disponible dans le serveur
        printf("%s\n", messageRecu);
        puts("\n☼☼☼ Choisissez un fichier ☼☼☼");
        printf("→ ");
        fgets(nomFichier, 50*sizeof(char),stdin);
        nomFichier[strlen(nomFichier) - 1]=0;
        printf("○ nom de fichier a envoyé %s ○\n", nomFichier);
        if (write(socket, nomFichier,50*sizeof(char)) == -1) {
        printf("%s",ROUGE);
        perror("[-]Erreur à l'envoie du nom de fichier. ");
        printf("%s\n",END);
        exit(1);
        }

        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        if (read(socket, messageRecu, longueurMessage*sizeof(char)) == -1) 
        {
            printf("%s",ROUGE);
            perror("[-]Erreur à la lecture de réponse de fichier si valide ou pas.");
            printf("%s\n",END);
            exit(1);
        }

        if(strcmp(messageRecu, "EnvoieValide") == 0)
        {
            // on recoie le fichier
            printf("\033[30;01m○ Le fichier existe bien !\033[00m ○\n");
            fichiersTrouver = 1;
            receptionFichier(socket);
            printf("\033[32;01m[+]Fichier a été bien reçu.\033[00m\n");
        } else 
        {
            // on redemande le nom de fichier
            printf("\033[30;01m○ Le fichier n'existe pas essaier un autre nom !\033[00m ○\n");
            fichiersTrouver = 0;
        }
    }
    // fin de l'ajout
    close(socket);
    pthread_exit(0);
}


/**
 * @brief Creation d'un nouveau canal de communication
 * Fonction qui permet la création d'un channel en envoyant à la suite les différentes informations
 * @param socket 
 */
void createChannel(){
    int socket = createNewSocket();

    // Tout d'abord on vérifie côté serveur si il est possible de créer un serveur
    char * reponse = malloc(100 * sizeof(char));
    switch(read(socket, reponse, 100*sizeof(char))){
        case -1:
            printf("%s",ROUGE);
            perror("[-] Erreur rencontrée dans l'envoi du nom du channel");
            printf("%s\n",END);
            exit(-1);
        case 0:
            printf("%s",ROUGE);
            perror("[-] La socket a été fermée par le serveur");
            printf("%s\n",END);
            exit(-1);
    }

    if(strcmp(reponse, "impossible") == 0){
        //on ne peut pas créer de channel
        printf("[!] Il ne reste plus de place pour créer un nouveau channel sur le serveur\n");
        return ;
    }

    // 1 ) On envoie le nom du channel
    char * name = malloc(100*sizeof(char));
    printf("%s▬▬ Quel est le nom que vous voulez donné au channel ? ▬▬%s\n",MAUVE,BLEU_CIEL);
    printf("→%s ",BLANC);
    fgets(name, 100*sizeof(char), stdin);
    name[strlen(name) - 1]=0;
    switch(write(socket, name, 100*sizeof(char))){
        case -1:
            printf("%s",ROUGE);
            perror("[-] Erreur rencontrée dans l'envoi du nom du channel");
            printf("%s\n",END);
            exit(-1);
        case 0:
            printf("%s",ROUGE);
            perror("[-] La socket a été fermée par le serveur");
            printf("%s\n",END);
            exit(-1);
    }

    // 2) Description du channel
    char * description = malloc(1024 * sizeof(char));
    printf("%s▬▬ Quelle est sa description ? ▬▬%s\n",MAUVE,BLEU_CIEL);
    printf("→%s ",BLANC);
    fgets(description,  1024 * sizeof(char), stdin);
    description[strlen(description) - 1]=0;
    switch(write(socket, description, 1024*sizeof(char))){
        case -1:
            printf("%s",ROUGE);
            perror("[-] Erreur rencontrée dans l'envoi de la description du channel");
            printf("%s\n",END);
            exit(-1);
        case 0:
            printf("%s",ROUGE);
            perror("[-] La socket a été fermée par le serveur");
            printf("%s\n",END);
            exit(-1);
    }

    // 3) Envoi de la taille maximum du channel
    char * tailleMax = malloc(5*sizeof(char));
    printf("%s▬▬ Quelle est sa taille maximum ? ▬▬%s\n", MAUVE,BLEU_CIEL);
    printf("→%s ",BLANC);
    fgets(tailleMax, 5*sizeof(char),stdin);
    tailleMax[strlen(tailleMax) - 1]=0;
    switch(write(socket, tailleMax, 5*sizeof(char))){
        case -1:
            printf("%s",ROUGE);
            perror("[-] Erreur rencontrée dans l'envoi de la taille du channel");
            printf("%s\n",END);
            exit(-1);
        case 0:
            printf("%s",ROUGE);
            perror("[-] La socket a été fermée par le serveur");
            printf("%s\n",END);
            exit(-1);
    }
    printf("%s○ le canal %s%s%s de taille %s%s%s et description : %s%s%s a été bien créer ○",MAUVE,BLANC,name,MAUVE,BLANC,tailleMax,MAUVE,BLANC,description,VERT);
}



/**
 * @brief 
 * Thread qui s'occupe des envois de messages du client au serveur
 * @param socketClient socket du Client qui permet la communication
 */
void * Envoyer(void * socketClient)
{
    int ecrits;
    int socket = (long)socketClient;
    char messageEnvoi[longueurMessage];
    pthread_t tFiles;

    // Envoie un message au serveur et gestion des erreurs
    printf("\n%s◘ CONNECTE ◘ \n",VERT);
    printf("%s○ Envoyer %s/help%s %spour voir la liste des commandes possibles ○\n",MAUVE,BLEU_CIEL,END, MAUVE);
    while(1)
    {
        memset(messageEnvoi, 0x00, longueurMessage*sizeof(char));
        printf("%s",BLEU_CIEL);
        puts("\n☼☼☼ Envoyer Un Message ☼☼☼");
        printf("%s→%s ", BLEU_CIEL,BLANC);
        fgets(messageEnvoi, longueurMessage*sizeof(char),stdin);
        messageEnvoi[strlen(messageEnvoi) - 1]=0;
        if(strcmp(messageEnvoi, "/file") == 0)
        {   
            procFichier(socket);
        } else if(strcmp(messageEnvoi, "/FilesRecup") == 0)
        {
            
            if (write(socket, "/FilesRecup",strlen("/FilesRecup")) == -1) {
            printf("%s",ROUGE);
            perror("[-]Error in sending file.");
            printf("%s\n",END);
            exit(1);
            }
            pthread_t tRecup;
            pthread_create(&tRecup,NULL, procRecupFichier,NULL);
            pthread_join(tRecup, NULL);
        }else if(strcmp(messageEnvoi, "/clear") == 0)
        {
            system("clear");
        }
         else 
        {    
            ecrits = write(socket, messageEnvoi,strlen(messageEnvoi));
            char ** separation;
            char * msg = malloc(longueurMessage*sizeof(char));
            strcpy(msg,messageEnvoi);
            separation = Separation(msg);
            switch(ecrits)
            {
                case -1: 
                    printf("%s",ROUGE);
                    perror("[-] Erreur dans l'envoi");
                    printf("%s\n",END);
                    close(socket);
                    exit(-3);
                case 0:
                    printf("%s",ROUGE);
                    fprintf(stderr, "[-] La socket a été fermée par le serveur !\n");
                    printf("%s\n",END);
                    close(socket);
                    exit(-5);
                default:
                    //gestion du cas où le client veut se déconnecter avec le mot "quit"
                    if(strcmp(messageEnvoi, "/fin") == 0){
                        printf("%s◘ CONNECTE ◘ \n",JAUNE);
                        printf("%s\n[!]---fin de la discussion---[!]\n", BLEU);
                        kill(getppid(), SIGTERM);
                        exit(0);
                    }
                    else if(strcmp(messageEnvoi, "/createChannel") == 0){
                        createChannel();
                    }
                    else if(strcmp(separation[0], "/modifChannel") == 0){
                        modifierCanal();
                    }
                    else{
                        printf("%s○ Message %s%s%s envoyé avec %ssuccés ☻ (%d octets) %s○\n",MAUVE,BLANC,messageEnvoi,MAUVE,BLEU_CIEL,ecrits,MAUVE);
                    }
            }
        }
    }
    pthread_exit(0);
}


/**
 * @brief 
 * Thread qui s'occupe de la réception des messages reçus par le serveur 
 * @param socketClient socket du Client qui permet la communication
 */
void * Recevoir(void * socketClient)
{
    // Reception des données du serveur
    int lus;
    char * messageRecu = malloc(longueurMessage*sizeof(char));
    int socket = (long)socketClient;
    while (1)
    {
        memset(messageRecu, 0x00, longueurMessage*sizeof(char));
        lus = read(socket, messageRecu, longueurMessage*sizeof(char));
        switch(lus)
        {
            case -1: 
                printf("%s",ROUGE);
                perror("[-] Erreur à la reception du message ");
                printf("%s\n",END);
                close(socket);
                exit(-4);
            case 0:
                printf("%s",ROUGE);
                printf("\n[-] La socket a été fermée par le serveur !\n");
                printf("%s\n",END);
                close(socket);
                exit(-5);
            default:
                if(strcmp(messageRecu, "/ban") == 0){
                    printf("%s◘ CONNECTE ◘ \n",JAUNE);
                    printf("%s[!]---fin de la discussion---[!]\n\n", BLEU);
                    if(write(socket, "/fin", 4*sizeof(char)) == -1)
                    {         
                        printf("%s",ROUGE);               
                        perror("[-] Erreur à l'envoie du message /fin");
                        printf("%s\n",END);
                     }
                    kill(getppid(), SIGTERM);
                    exit(0);
                }else if(strcmp(messageRecu, "/STOP") == 0){
                    printf("%s◘ CONNECTE ◘ \n",JAUNE);
                    printf("%s[!]---fin de la discussion---[!]\n\n", BLEU);
                    if(write(socket, "/fin", 4*sizeof(char)) == -1)
                    {         
                        printf("%s",ROUGE);               
                        perror("[-] Erreur à l'envoie du message /fin");
                        printf("%s\n",END);
                     }
                    kill(getppid(), SIGTERM);
                    exit(0);
                }              
                printf("\n");
                puts(messageRecu);
                // On a fini d'afficher le message recu on affiche la demande d'envoie
                printf("%s",BLEU_CIEL);
                printf("☼☼☼ Envoyer Un Message ☼☼☼%s\n",BLANC);
        }
    }
    pthread_exit(0);   
}




int main(int argc, char *argv[]) 
{
    printf("%s \n", BLANC);
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
    printf("○ la Socket a été %sbien créé ! (%ld) %s○\n",BLEU_CIEL,socketClient,BLANC);
    
    
    longueurAdresse = sizeof(pointDeRencontreDistant);
    // Initialise à 0 la struct sockaddr_in
    memset(&pointDeRencontreDistant, 0x00, longueurAdresse);

    // Renseigne la structure sockaddr_in avec les informations du serveur distant
    pointDeRencontreDistant.sin_family = PF_INET;
    ip = malloc(50*sizeof(char));
    
    port = atoi(argv[2]);
    pointDeRencontreDistant.sin_port =  htons(port);
    strcpy(ip, argv[1]);
    inet_aton(ip,&pointDeRencontreDistant.sin_addr);

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
    printf("○ Connection de la socket avec the server %sréussi %s○\n\n",BLEU_CIEL,BLANC);

    printf("%s◘ CONNECTE ◘ %s\n",JAUNE,BLANC);
    connection(socketClient);    

    pthread_create(&tRecepteur, NULL, Recevoir, (void *) socketClient);
    pthread_create(&tEcouter, NULL, Envoyer, (void *) socketClient);
    pthread_join(tEcouter, NULL);    
    pthread_join(tRecepteur, NULL);

    printf("%s\n", END);
    // On a fini on coupe la ressource pour quitter
    close(socketClient);
    return 0;
}