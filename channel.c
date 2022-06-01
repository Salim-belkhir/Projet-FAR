#include <stdio.h> 
#include <stdlib.h>
#include "channel.h"
#include <string.h>

/**
 * @brief 
 * Fonction qui crée un Channel vide
 * @param name Nom du channel
 * @param descr Description du channel
 * @param capacity Capacité maximale de clients qui peuvent être acceptés
 * @return Channel* 
 */
Channel * cree_Channel(char * name, char* descr,int capacity){
    if(capacity < 0){
        printf("Cette valeur n'est pas valide, la capacité doit être supérieure à 0\n");
    }
    Channel* channel = malloc(sizeof(Channel*));
    channel->nom = malloc(100*sizeof(char));
    channel->description = malloc(128*sizeof(char)); 
    strcpy(channel->nom,name);
    strcpy(channel->description,descr); 
    channel->count = 0;
    channel->capacity = capacity;
    channel->clients = malloc(capacity*sizeof(int));
    // le tableau des clients est initialiser à -1 Aucun client n'est enregistrer.
    for(int i=0; i<capacity; i++) 
    {
        channel->clients[i] = -1;
    }
    return channel;
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
 * Verifie si un client est connecté a un channel à l'aide de son identifiant
 * @param channel le channel pour lequel on vérifie
 * @param id l'identifiant du client
 * @return Retourne 1 si le client appartient au serveur, sinon 0 
 */
int appartenir(Channel * channel, int id){
    int nonFini = 1;
    int trouve = 0;
    int i = 0;
    while(i < channel->count && nonFini){
        if(channel->clients[i] == id){
            trouve = 1;
            nonFini = 0;
        }
        i++;
    }
    return trouve;
}

/**
 * @brief 
 * modifie le nom du channel passé en paramètres 
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param newName nouveau nom du channel
 */
void setName(Channel * channel, char * newName){
    strcpy(channel->nom,newName);
}

/**
 * @brief 
 * modifie la description du channel
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param newDescr nouvelle description du channel
 */
void setDescription(Channel * channel, char * newDescr){
    strcpy(channel->description,newDescr);
}


void setCapacity(Channel * channel,int capacity){
    channel->capacity = capacity;
}

/**
 * @brief 
 * fonction qui ajoute un client à un Channel
 * @param channel channel sur lequel il faut supprimer un utilisateur
 * @param id id du client à ajouter
 * @return retourne 1 si l'ajout est un succès, sinon -1
 */
int ajouter_client(Channel * channel, int id){
    if(channel->count >= channel->capacity){
        printf("\033[31;01m[!] Le channel a atteint la limite des clients possibles\033[00m\n");
        return -1;
    }        
    int i =0;
    int nonInsere = 1;
    while(i<channel->capacity && nonInsere){
        if(channel->clients[i] == -1){
            channel->clients[i] = id;
            nonInsere = 0;
        }
        i++;
    }
    channel->count ++;
    return 1;
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
    } else {

        int nonSupprime = 1;
        int i = 0; 
        while(i<channel->capacity && nonSupprime){
            if(channel->clients[i] == id){
                channel->clients[i] = -1;
                nonSupprime = 0;
                channel->count--;
            }
            i++;
        }
        if(nonSupprime == 0){
            printf("Le client a bien été déconnecté du channel\n");
        }
        else{
            perror("Le client n'est pas connecté au channel \n");
            printf("%s", channel->nom);
            exit(-1);
        }

        i--;
        while(channel->clients[i] != -1){
            channel->clients[i-1] = channel->clients[i];
            channel->clients[i] = -1;
            i++;
        }
    }
}

/**
 * @brief 
 * Affiche la liste des clients connecté au channel
 * @param channel channel sur lequel les clients sont connecté 
 */
void afficheClients(Channel * channel)
{
    int i;
    printf("\033[36,01mLa liste des clients : \033[00m\n");
    for(i=0; i<channel->count; i++)
    {
        printf("→\033[30;01m id :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n", channel->clients[i]);
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
    int i;
    int boucle = channel-> count;
    for(i = 0; i < boucle; i++)
    {
        supprimer_client(channel,channel->clients[i]);
    }
}

/*

int main(int argc, char const *argv[])
{
    printf ("\033[36;01mMain test de Channel\033[00m\n");
    // Création d'un Channel
    Channel * cha1= cree_Channel("Channel1","Tester channel1",6);
    
    printf("\033[30;01mLe channel est vide ?\033[00m\033[32;01m %d\033[00m\n",Channel_est_vide(cha1));

    printf("\033[30;01mLe nom du channel :\033[00m \033[36;01m< \033[00m\033[32;01m%s\033[00m\033[36;01m >\033[00m\n",getName(cha1));

    printf("\033[30;01mla description du channel :\033[00m \033[36;01m< \033[00m\033[32;01m%s\033[00m\033[36;01m >\033[00m\n",getDescription(cha1));

    printf("\033[30;01mla capacité du channel  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCapacity(cha1));

    setName(cha1, "Chouchou$");

    setDescription(cha1, "Test de channel");

    printf("\033[30;01mLe channel est vide ?\033[00m\033[32;01m %d\033[00m\n",Channel_est_vide(cha1));

    printf("\033[30;01mLe nom du channel :\033[00m \033[36;01m< \033[00m\033[32;01m%s\033[00m\033[36;01m >\033[00m\n",getName(cha1));

    printf("\033[30;01mla description du channel :\033[00m \033[36;01m< \033[00m\033[32;01m%s\033[00m\033[36;01m >\033[00m\n",getDescription(cha1));

    printf("\033[30;01mNombre de client connecté est  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCount(cha1));
    ajouter_client(cha1, 1);
    ajouter_client(cha1, 2);
    ajouter_client(cha1, 3);
    ajouter_client(cha1, 4);
    ajouter_client(cha1, 5);
    ajouter_client(cha1, 6);
    ajouter_client(cha1, 7);
    afficheClients(cha1);
    printf("\033[30;01mNombre de client connecté est  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCount(cha1));

    supprimer_client(cha1, 1);
    supprimer_client(cha1, 2);
    supprimer_client(cha1, 3);
    supprimer_client(cha1, 4);
    supprimer_client(cha1, 5);
    supprimer_client(cha1, 6);
    supprimer_client(cha1, 7);

    afficheClients(cha1);
    printf("\033[30;01mNombre de client connecté est  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCount(cha1));    


    ajouter_client(cha1, 1);
    ajouter_client(cha1, 2);
    ajouter_client(cha1, 3);
    ajouter_client(cha1, 4);
    ajouter_client(cha1, 5);
    afficheClients(cha1);
    printf("\033[30;01mNombre de client connecté est  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCount(cha1));


// la fonction deconnecterTousClients ne fonctione pas correctement il faut la regler
    deconnecterTousClients(cha1);
    printf("\033[30;01mLe channel est vide ?\033[00m\033[32;01m %d\033[00m\n",Channel_est_vide(cha1));
    printf("\033[30;01mNombre de client connecté est  :\033[00m \033[36;01m< \033[00m\033[32;01m%d\033[00m\033[36;01m >\033[00m\n",getCount(cha1));    

//int * getClients(Channel * channel);

/*
    //void supprimer_Channel(Channel * Channel);

    return 0;

}
*/