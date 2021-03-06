typedef struct Channel Channel;

struct Channel{
    char * nom;
    char * description;
    int capacity;
    int count;
    int * clients;
};

Channel * cree_Channel(char * name, char* descr,int capacity);

int Channel_est_vide(Channel * channel);

char * getName(Channel * channel);

char * getDescription(Channel * channel);

int getCapacity(Channel * channel);

int getCount(Channel * channel);

int* getClients(Channel * channel);

void setName(Channel * channel, char * newName);

void setDescription(Channel * channel, char * newDescr);

void setCapacity(Channel * channel,int capacity);

int appartenir(Channel * channel, int id);

int ajouter_client(Channel * channel, int id);

void afficheClients(Channel * channel);

void supprimer_client(Channel * channel, int id);

void deconnecterTousClients(Channel * channel);