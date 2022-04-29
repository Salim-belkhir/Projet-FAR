typedef struct Element Element;

struct Element
{
    int id;
    Element *suivant;
    Element * precedent;
    char * chaine;
};

typedef struct liste liste;
struct liste
{
    Element * premier;
};

liste * cree_liste();

int liste_est_vide(liste * l);

void ajouter_debut(liste * l, int id, char * chaine);

void supprimer_debut(liste * l);

void ajouter_fin(liste * l, int id, char * chaine);

void ajouter_fin_recur(Element *element,int id, char * chaine);

void supprimer_fin(liste * l);

void supprimer_fin_recur(Element *element);

int Taille(liste *liste);

Element *  afficherListe(liste * l,Element * selectionne);

Element * itemSuivant(Element * l);

Element * itemPrecedent(Element * l);

void Options_Liste();