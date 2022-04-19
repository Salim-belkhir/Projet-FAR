typedef struct Element Element;
struct Element
{
    int nombre;
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

void ajouter_debut(liste * l, int val, char * chaine);

void supprimer_debut(liste * l);

void ajouter_fin(liste * l, int val, char * chaine);

void ajouter_fin_recur(Element *element, int val, char * chaine);

void supprimer_fin(liste * l);

void supprimer_fin_recur(Element *element);

Element *  afficherListe(liste * l,Element * selectionne);



int map(int n, int (*f) (int));
int doubler(int n);
int div2(int n);
void dbl(liste * l , int (*f) (int));
void dbl_recur(Element * element, int (*f) (int));
Element * itemSuivant(Element * l);
Element * itemPrecedent(Element * l);
void Options_Liste();