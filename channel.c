#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "channel.h"


/**
 * @brief 
 * Fonction qui crée un Channel vide
 * @return Channel* 
 */
Channel * cree_Channel(){
    Channel* channel = malloc(sizeof(Channel*));
    channel->nom = malloc(50*sizeof(char));
    channel->description = malloc(128*sizeof(char)); 
    channel->count = 0;
    channel->capacity = 0;
    channel->clients = cree_liste();
    return channel;
}

/**
 * @brief 
 * fonction qui permet de savoir si un channel est vide, c'est-à-dire pas de clients connectés
 * @param channel channel de communication sur lequel il faut vérifier s'il est vide 
 * @return Retourne 1 si il n'y a pas de clients connectés, sinon 0
 */
int Channel_est_vide(Channel * channel){
    if(channel->count == 0){
        return 1;
    }
    return 0;
}

/**
 * @brief 
 * Retourne le nom du channel
 * @param channel channel de communication sur lequel il faut récupèrer son nom  
 * @return char* 
 */
char * getName(Channel * channel){
    return channel->nom;
}

/**
 * @brief 
 * Retourne la description du channel
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @return char* 
 */
char * getDescription(Channel * channel){
    return channel->description;
}

/**
 * @brief 
 * retourne la capacité maximale de clients dans un channel
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @return int 
 */
int getCapacity(Channel * channel){
    return channel->capacity;
}

/**
 * @brief 
 * récupère le nombre de clients connectés
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @return int 
 */
int getCount(Channel * channel){
    return channel->count;
}

/**
 * @brief 
 * retourne le pointeur vers la liste des clients
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @return liste* 
 */
liste * getClients(Channel * channel){
    return channel->clients;
}

/**
 * @brief 
 * Affiche la liste des clients connecté au channel
 * @param channel channel sur lequel les clients sont connecté 
 */
void afficheClients(Channel * channel)
{
    Element * actuel = channel->clients->premier;
    printf("\033[36,01mLa liste des clients : \033[00m\n");
    while(actuel -> suivant != NULL)
    {
        printf("→ \033[30;01m Pseudo :\033[00m \033[36;01m< \033[00m\033[32;01m%s\033[00m\033[36;01m >\033[00m\033[30;01m id :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n", actuel -> pseudo, actuel ->id);
        actuel = actuel -> suivant;
    }
}

/**
 * @brief 
 * modifie le nom du channel passé en paramètres 
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param newName nouveau nom du channel
 */
void setName(Channel * channel, char * newName){
    channel->nom = newName;
}

/**
 * @brief 
 * modifie la description du channel
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param newDescr nouvelle description du channel
 */
void setDescription(Channel * channel, char * newDescr){
    channel->description = newDescr;
}

/**
 * @brief 
 * modifie la capacite du channel
 * @param channel channel de communication
 * @param newDescr nouvelle capacite du channel
 */
void setCapicite(Channel * channel, int capacite)
{
    if(capacite <= 0){
        perror("Cette valeur n'est pas valide, la capacité doit être supérieure à 0\n");
        exit(-1);
    }
    channel->capacity = capacite;
}

/**
 * @brief 
 * fonction qui ajoute un client à un Channel
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param id id du client à ajouter
 */
void ajouter_client(Channel * channel, int id, char * pseudo){
    if(channel->count >= channel->capacity){
        printf("\033[31;01m[!] Le channel a atteint la limite des clients possibles\033[00m\n");
    } else {        
        ajouter_debut(channel->clients, id, pseudo);
        channel->count++;
    }
}

/**
 * @brief 
 * fonction qui permet de déconnecter un client d'un channel à partir de son id
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param id identifiant du client qu'il faut déconnecter 
 */
void supprimer_client(Channel * channel, int id){
    if(Channel_est_vide(channel)){
        printf("\033[33;01m[!] Le channel est vide, il n'y a aucun client à supprimer du channel\033[00m\n");
    }else
    {            
        supprimer_val(channel->clients,id);
        channel->count--;
    }
}

//A faire ailleurs, car il suffit de mettre a null la case qui contiendra ce channel
//void supprimer_Channel(Channel * Channel){}

/**
 * @brief 
 * Deconnecte tous les clients (recrée un nouveau channel)
 * @param channel channel sur lequel il faut supprimer un utilisateur
 */
void deconnecterTousClients(Channel * channel){
    Element * actuel = channel->clients->premier;
    while(actuel -> suivant != NULL)
    {
        printf("actule %d \n", actuel -> id);
        supprimer_client(channel, actuel ->id);
        actuel = actuel->suivant;
    }
}

int main(int argc, char const *argv[])
{
    printf ("\033[36;01mMain test de Channel\033[00m\n");
    // Création d'un Channel
    Channel * cha1= cree_Channel();
    setName(cha1, "Channel1");

    setDescription(cha1, "Test de channel");

    setCapicite(cha1,5);
    printf("\033[30;01mLa chaine est vide ?\033[00m\033[32;01m %d\033[00m\n",Channel_est_vide(cha1));

    printf("\033[30;01mLe nom du channel :\033[00m \033[36;01m< \033[00m\033[32;01m%s\033[00m\033[36;01m >\033[00m\n",getName(cha1));

    printf("\033[30;01mla description du channel :\033[00m \033[36;01m< \033[00m\033[32;01m%s\033[00m\033[36;01m >\033[00m\n",getDescription(cha1));

    printf("\033[30;01mla capacité du channel  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCapacity(cha1));

    printf("\033[30;01mNombre de client connecté est  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCount(cha1));
    ajouter_client(cha1, 1, "ayoub");
    ajouter_client(cha1, 2, "bibo");
    ajouter_client(cha1, 3, "salim");
    ajouter_client(cha1, 4, "toto");
    ajouter_client(cha1, 5, "titi");
    afficheClients(cha1);

/*
    supprimer_client(cha1, 1);
    supprimer_client(cha1, 2);
    supprimer_client(cha1, 3);
    afficheClients(cha1);
    supprimer_client(cha1, 4);
    supprimer_client(cha1, 5);
    supprimer_client(cha1, 6);
*/
    // la fonction deconnecterTousClients ne fonctione pas correctement il faut la regler
    deconnecterTousClients(cha1);
    printf("\033[30;01mLa chaine est vide ?\033[00m\033[32;01m %d\033[00m\n",Channel_est_vide(cha1));

//liste * getClients(Channel * channel);

/*
    //void supprimer_Channel(Channel * Channel);
*/
    return 0;
}

