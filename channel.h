#include "liste.h"
typedef struct Channel Channel;

struct Channel{
    char * nom;
    char * description;
    int capacity;
    int count;
    liste * clients;
};

Channel * cree_Channel();

int Channel_est_vide(Channel * channel);

void  afficheClients(Channel * channel);

char * getName(Channel * channel);

char * getDescription(Channel * channel);

int getCapacity(Channel * channel);

int getCount(Channel * channel);

liste * getClients(Channel * channel);

void setName(Channel * channel, char * newName);

void setDescription(Channel * channel, char * newDescr);

void setCapicite(Channel * channel, int capacite);

void ajouter_client(Channel * channel, int id, char * pseudo);

void supprimer_client(Channel * channel, int id);

//void supprimer_Channel(Channel * Channel);

void deconnecterTousClients(Channel * channel);