#include <stdio.h> 
#include <stdlib.h>
#include "channel.h"

/**
 * @brief 
 * Fonction qui crée un Channel vide
 * @param name Nom du channel
 * @param descr Description du channel
 * @param capacity Capacité maximale de clients qui peuvent être acceptés
 * @return Channel* 
 */
Channel * cree_Channel(char * name, char* descr,int capacity){
    if(capacity <= 0){
        perror("Cette valeur n'est pas valide, la capacité doit être supérieure à 0\n");
        exit(-1);
    }
    Channel* channel = malloc(sizeof(Channel*));
    channel->nom = name;
    channel->description = descr; 
    channel->count = 0;
    channel->capacity = capacity;
    channel->clients[capacity];
    int i = 0;
    for(i; i < capacity; i++){
        channel->clients[i] = NULL;
    }
}

/**
 * @brief 
 * fonction qui permet de savoir si un channel est vide, c'est-à-dire pas de clients connectés
 * @param channel channel sur lequel il faut supprimer un utilisateur
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
 * @param channel channel sur lequel il faut supprimer un utilisateur 
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
 * retourne le pointeur vers le tableau des clients
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @return int* 
 */
int* getClients(Channel * channel){
    return channel->clients;
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
 * fonction qui ajoute un client à un Channel
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param id id du client à ajouter
 */
void ajouter_client(Channel * channel, int id){
    if(channel->count >= channel->capacity){
        perror("[!] Le channel a atteint la limite des clients possibles\n");
        exit(-1);
    }
    int i =0;
    int nonInsere = 1;
    while(i<channel->capacity && nonInsere){
        if(channel->clients[i] == NULL){
            channel->clients[i] = id;
            nonInsere = 0;
        }
        i++;
    }
    channel->count ++;
}

/**
 * @brief 
 * fonction qui permet de déconnecter un client d'un channel à partir de son id
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param id identifiant du client qu'il faut déconnecter 
 */
void supprimer_client(Channel * channel, int id){
    if(Channel_est_vide(channel)){
        perror("[!] Le channel est vide, il n'y a aucun client à supprimer du channel\n");
        exit(-1);
    }
    int nonSupprime = 1;
    int i = 0; 
    while(i<channel->capacity && nonSupprime){
        if(channel->clients[i] == id){
            channel->clients[i] = NULL;
            nonSupprime = 0;
            channel->count--;
        }
        i++;
    }
    if(nonSupprime == 0){
        printf("Le client a bien été déconnecté du channel");
    }
    else{
        perror("Le client n'est pas connecté au channel \n");
        printf("%s", channel->nom);
        exit(-1);
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
    Channel * channel = cree_Channel(channel->nom, channel->description, channel->capacity);
    free(channel->clients);
    channel = channel;
}