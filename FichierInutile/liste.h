typedef struct liste liste;

struct liste{
    int id;   
    char * pseudo;  
    liste * suivant;
    liste * precedent;
};

liste * creer_liste();

int liste_est_vide(liste * l);

void ajouter_debut(liste * l, int id, char * chaine);

liste * supprimer_debut(liste * l);

liste * ajouter_fin(liste * l, int id, char * chaine);

liste * supprimer_fin(liste * l);

void supprimer_val(liste * l, int val);

int taille_liste(liste * l);

void afficherListe(liste * l);