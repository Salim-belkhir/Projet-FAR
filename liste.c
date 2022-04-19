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

    element->nombre = 0;
    element->suivant = NULL;
    element -> precedent = NULL;
    element -> chaine = (char*)(malloc(sizeof(char)));
    element -> chaine = "oran";
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
    element -> nombre = val;
    element -> chaine = chaine;    
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


//fonction qui ajoute l'élément passé en paramètre à la fin de la liste
void ajouter_fin(liste * l, int val, char * chaine)
{   
    ajouter_fin_recur(l -> premier, val, chaine);
}

//fonction auxiliaire de ajouter_fin qui parcourt la liste
void ajouter_fin_recur(Element *element, int val, char * chaine)
{
    if (element -> suivant -> suivant == NULL)
    {
        Element * inserer = malloc(sizeof(*inserer));
        inserer -> suivant = element -> suivant;
        inserer-> precedent = element;
        inserer -> nombre = val;
        element->suivant->precedent = inserer;
        inserer -> chaine = chaine;
        element -> suivant = inserer; 
    }
    else
    {
        ajouter_fin_recur(element -> suivant, val,chaine);
    }
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

//fonction qui return la liste
//affiche une erreur si la liste passé en paramètre est vide 
Element * afficherListe(liste *liste, Element * selectionne)
{
    if (liste == NULL)
    {
        exit(EXIT_FAILURE);
    }

    Element *actuel = liste->premier;
    Element * select;
    printf("  NULL <-- \n");
    while (actuel->suivant != NULL)
    {
        if(selectionne == actuel)
        {
            printf("->");
            select = actuel;
        }
        printf(" [ %s ] \n", actuel->chaine);
        actuel = actuel->suivant;
    }
    printf("  --> NULL\n");
    return select;
}

//fonction qui premet de récuperer l'élément à un certain indice 
int map(int n, int (*f) (int))
{
    return f(n);
}

//fonction double un élément de la liste passé en paramètre 
int doubler(int n)
{
    return n*2;
}

//fonction qui divise un élément par 2 passé en paramètre 
int div2(int n)
{
    return (int) n/2;
}


void dbl(liste * l , int (*f) (int))
{
    dbl_recur(l->premier, f);
}

//parcours de toute la liste afin de doubler tous les éléments de la liste 
void dbl_recur(Element * element, int (*f) (int))
{
    if(element -> suivant -> suivant == NULL)
    {
        element-> nombre = map(element->nombre, f); 
    }
    else
    {
        element-> nombre = map(element->nombre, f);
        dbl_recur(element -> suivant, f);
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

int main(int argc, char const *argv[])
{
    
    liste * l = cree_liste();

    //int estvide = liste_est_vide(l);
    //printf("%d\n", estvide); // elle dois renvoier 1 car la liste est vide

    ajouter_debut(l , 10, "item1");
    ajouter_fin(l , 20,"item2");
    ajouter_fin(l , 30,"item3");
    ajouter_fin(l , 40,"item4");
    
    // supprimer_fin(l);
    // supprimer_debut(l);
    
    // printf("%d \n", l->premier->suivant->precedent->nombre);
    // printf("%d \n", l->premier->suivant->suivant->precedent->nombre);

    //printf("%d, \n", itemSuivant(l->premier));
    //printf("%d, \n", itemPrecedent(l->premier->suivant->suivant));

    printf("est ce que la liste est vide ?\n");
    if ( liste_est_vide(l) == 1)
    {
        printf("la liste est vide ! \n");
    } else { 
        printf("la liste contient au moin  un element \n");
    }

    /*
    int (*pointeur_doubler) (int) = doubler; // ou bien div2
    dbl(l, pointeur_doubler);
    */ 
    printf(" Bonjour, ceci est une liste doublement chainé à 4 éléments \n");
    
    system("clear");
    Element * maillon;
    maillon = afficherListe(l,l->premier);

    int fin = 1; 
    while( fin == 1)
    {
        printf("\n");
        Options_Liste();

        int choix1 = 0; 
        do
        {
            printf("Votre Choix?\n");
            scanf("%d", &choix1);
        }while(choix1 < 1 || choix1 > 4);

        switch (choix1)
        {
            case 1:
                system("clear");
                maillon = afficherListe(l,itemSuivant(maillon));
                break;
            case 2:
                system("clear");
                maillon = afficherListe(l,itemPrecedent(maillon));
                break;
            case 3:
                printf(" Le nombre du maillon est %d \n", maillon->nombre);
                break;
            case 4:
                fin = 2;
                system("clear");
                printf("By by ! \n");
                exit(EXIT_FAILURE);
                break;
            default:
                printf("\n Oups mauvais choix \n");
                break;
        }
    }
    return 0;
}