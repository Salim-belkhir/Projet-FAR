#include <stdio.h> 
#include <stdlib.h>
#include "liste.h"

//fonction de création d'une liste vide 
liste * cree_liste()
{
    liste * Liste = malloc(sizeof(*Liste));
    Element *element = malloc(sizeof(*element));

    if (Liste == NULL || element == NULL)
    {
        exit(EXIT_FAILURE);
    }

    element->id = 0;
    element->suivant = NULL;
    element -> precedent = NULL;
    element -> pseudo = NULL;
    Liste->premier = element;
    return Liste;
}

//fonction qui vérifie si une liste est vide
//renvoi 1 si la liste est vide, et 0 sinon
int liste_est_vide(liste * l)
{
    int result = 0;
    if(l -> premier -> suivant == NULL)
    {
        result = 1;
    }
    return result;
}

//fonction qui ajoute l'élément passé en paramètre au début de la liste
void ajouter_debut(liste * l, int val, char * chaine)
{
    Element *element = malloc(sizeof(*element));
    element -> id = val;
    element -> pseudo = chaine;    
    element -> precedent = NULL;
    element -> suivant = l -> premier;
    element -> suivant -> precedent = element;
    l-> premier = element;
}

//fonction qui supprime le premier élément de la liste
void supprimer_debut(liste * l)
{
    /*verfier si la liste est vide*/
    Element * Asupprimer = l -> premier;
    l -> premier = Asupprimer -> suivant;
    Asupprimer->suivant->precedent = l->premier;
    free(Asupprimer);
}

//fonction qui supprime l'élément en fin de liste, ne supprime rien si liste vide
void supprimer_fin(liste * l)
{
    supprimer_fin_recur(l -> premier);
}

//fonction auxiliaire de supprimer_fin qui parcours la liste
void supprimer_fin_recur(Element *element)
{
    if (element -> suivant -> suivant -> suivant == NULL)
    {
        Element * Asupprimer = element -> suivant;
        element -> suivant->suivant->precedent = element; 
        element -> suivant =  Asupprimer -> suivant;
        free(Asupprimer); 
    }
    else
    {
        supprimer_fin_recur(element -> suivant);
    }
}

//fonction qui supprime l'élément en fin de liste, ne supprime rien si liste vide
void supprimer_val(liste * l, int val)
{
    if(liste_est_vide(l) == 1)
    {
        printf("la liste est vide !\n");
    }
    else if(l->premier->precedent == NULL && l->premier->id == val)
    {
        supprimer_debut(l);
    } 
    else {
        supprimer_val_recur(l -> premier, val);
    }
}

//fonction auxiliaire de supprimer_fin qui parcours la liste
void supprimer_val_recur(Element * element,int val)
{
    if(element -> id == val)
    {    
        if (element -> suivant != NULL && element -> precedent != NULL)
        {
            element -> suivant ->precedent = element -> precedent; 
            element -> precedent -> suivant =  element -> suivant;
            free(element); 
        }
        else if (element -> suivant == NULL)
        { 
            element -> precedent -> suivant =  NULL;
            free(element); 
        }
    }
    else
    {
        supprimer_val_recur(element -> suivant, val);
    }
}

//fonction qui return la liste
//affiche une erreur si la liste passé en paramètre est vide 
void afficherListe(liste *liste)
{
    if (liste == NULL)
    {
        exit(EXIT_FAILURE);
    }
    Element * actuel = liste->premier;
    while (actuel->suivant != NULL)
    {
        printf("le client %s a pour identifiant -> %d\n", actuel->pseudo, actuel->id);
        actuel = actuel->suivant;
    }
}

//foncton qui retourne l'élément suivant, si il y'en a pas, return l'élément 
Element * itemSuivant(Element * l)
{
    if (l -> suivant -> suivant == NULL)
    {
        return l;
    }
    else
    {
        return l -> suivant;
    }
}

//fonction qui return l'élément precedent, si il y'en a pas, return l'élément 
Element * itemPrecedent(Element * l)
{
    if (l -> precedent  == NULL)
    {
        return l;
    }
    else
    {
        return l -> precedent;
    }
}

void Options_Liste()
{
	printf(" ======================================================\n");
	printf(" Les choix Possibles :\n");
    printf(" Choix 1 : Sélectionner item suivant\n");
    printf(" Choix 2 : Sélectionner item précédent\n");
    printf(" Choix 3 : Afficher la valeur du maillon séléctionné\n");
    printf(" Choix 4 : 'Quitter'\n\n");
}


int liste_taille(liste * l)
{
    int taille = 0;
    Element * actuel = l -> premier;
    while(actuel->suivant != NULL)
    {
        taille++;
        actuel = actuel->suivant;
    }
    return taille;
}

/*
int main(int argc, char const *argv[])
{
    liste * l = cree_liste();
    int estvide = liste_est_vide(l);
    printf("est ce que la liste est vide ? → %d\n", estvide); // elle dois renvoier 1 car la liste est vide

    ajouter_debut(l , 1, "ayoub");
    estvide = liste_est_vide(l);
    liste_taille(l);
    printf("est ce que la liste est vide ? → %d\n", estvide); // elle dois renvoier 0 car la liste est non vide
    
    ajouter_debut(l , 2, "salim");
    estvide = liste_est_vide(l);
    liste_taille(l);
    printf("est ce que la liste est vide ? → %d\n", estvide); // elle dois renvoier 0 car la liste est non vide

    ajouter_debut(l , 3, "bibo");
    estvide = liste_est_vide(l);
    liste_taille(l);
    printf("est ce que la liste est vide ? → %d\n", estvide); // elle dois renvoier 0 car la liste est non vide

    afficherListe(l);

    printf("On commence la supression ! \n");
    supprimer_val(l,1);
    supprimer_val(l,2);
    supprimer_val(l,3);
    ajouter_debut(l , 3, "bibo");
ajouter_debut(l , 3, "bibo");
    estvide = liste_est_vide(l);
    printf("la taille de liste %d\n",liste_taille(l));
    printf("est ce que la liste est vide ? → %d\n", estvide); // elle dois renvoier 0 car la liste est non vide
    afficherListe(l);

    return 0;
}
*/