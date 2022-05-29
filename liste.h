typedef struct Element Element;
struct Element
{
    int id;
    Element *suivant;
    Element * precedent;
    char * canal;
    char * pseudo;
    int admin; 
};

typedef struct liste liste;

struct liste
{
    Element * premier;
};

liste * cree_liste();

int liste_est_vide(liste * l);

void ajouter_debut(liste * l, int val, char * chaine, char * canal, int admin);

char * getCanalClient(liste * li, int id);

int modifierCanalClient(liste * l, int id, char * canal);

void supprimer_debut(liste * l);

void supprimer_fin(liste * l);

void supprimer_fin_recur(Element *element);

void supprimer_val(liste * l, int val);

void supprimer_val_recur(Element *element, int val);

void afficherListe(liste * l);

int liste_taille(liste * l);

Element * itemSuivant(Element * l);

Element * itemPrecedent(Element * l);