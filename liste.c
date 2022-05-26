#include <stdio.h> 
#include <stdlib.h>
#include "liste.h"
#include <string.h>



/**
 * @brief 
 * fonction de création d'une liste vide
 * @return liste*  Pointeur vers la nouvelle structure vide
 */
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
    element -> canal = malloc(100*sizeof(char));
    Liste->premier = element;
    return Liste;
}


/**
 * @brief 
 * fonction qui vérifie si une liste est vide
 * @param l pointeur vers la structure liste
 * @return renvoie 1 si la liste est vide, sinon 0 
 */
int liste_est_vide(liste * l)
{
    int result = 0;
    if(l -> premier -> suivant == NULL)
    {
        result = 1;
    }
    return result;
}


/**
 * @brief 
 * Retourne le nom du channel auquel un client est connecté
 * @param li Liste des clients
 * @param id Identifiant du client dont on cherche le canal
 * @return Nom du channel 
 */
char * getCanalClient(liste * li, int id){
    if(liste_est_vide(li)){
        perror("[!] La liste des utilisateurs est vide");
        exit(-1);
    }
    char * channel = malloc(100*sizeof(char));
    int nonTrouve = 1;
    Element * e = li->premier;
    while(e!= NULL && nonTrouve){
        if(e->id == id){
            nonTrouve = 0;
            strcpy(channel, e->canal);
        }
        e = e->suivant;
    }
    if(e == NULL || nonTrouve){
        perror("Client non existant");
        exit(-1);
    }
    return channel;
}


/**
 * @brief 
 * Modifie le canal associer à un client
 * @param l 
 * @param id 
 * @param canal 
 * @return retourne 1 si le canal est mis à jour, sinon -1
 */
int modifierCanalClient(liste * l, int id, char * canal){
    if(liste_est_vide(l)){
        return -1;
    }
    int taille = liste_taille(l);
    int i = 0;
    int nonFini = 1;
    Element * e = l->premier;
    while(e != NULL && nonFini){
        if(e->id == id){
            strcpy(e->canal, canal);
            nonFini = 0;
        }
        e = e->suivant;
    }
    if(nonFini){
        return -1;
    }
    return 1;
}


/**
 * @brief 
 * fonction qui ajoute l'élément passé en paramètre au début de la liste
 * @param l Le pointeur sur la structure liste 
 * @param id L'identifiant du client à ajouter
 * @param pseudo Le pseudo du client 
 */
void ajouter_debut(liste * l, int id, char * pseudo, char * canal)
{
    Element *element = malloc(sizeof(*element));
    element -> id = id;
    element -> pseudo = pseudo;
    element -> canal = canal;    
    element -> precedent = NULL;
    element -> suivant = l -> premier;
    element -> suivant -> precedent = element;
    l-> premier = element;
}


/**
 * @brief 
 * fonction qui supprime le premier élément de la liste
 * @param l Le pointeur sur la structure liste 
 */
void supprimer_debut(liste * l)
{
    //on vérifie que la liste n'est pas vide
    if(liste_est_vide(l)){
        printf("La liste est vide, il n'y a rien à supprimer\n");
        return ;
    }
    Element * Asupprimer = l -> premier;
    l -> premier = Asupprimer -> suivant;
    Asupprimer->suivant->precedent = l->premier;
    free(Asupprimer);
}


/**
 * @brief 
 * fonction qui supprime l'élément en fin de liste, ne supprime rien si liste vide
 * @param l Le pointeur sur la structure de la liste 
 */
void supprimer_fin(liste * l)
{
    if(liste_est_vide(l)){
        printf("Il n'y a rien à supprimer la liste est vide");
        return ;
    }
    supprimer_fin_recur(l -> premier);
}


/**
 * @brief 
 * fonction auxiliaire de supprimer_fin qui parcours la liste
 * @param element La liste des clients récursive
 */
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


/**
 * @brief 
 * fonction qui supprime l'élément en fin de liste, ne supprime rien si liste vide
 * @param l Le pointeur de la structure liste 
 * @param id L'identifiant du client à supprimer
 */
void supprimer_val(liste * l, int id)
{
    if(liste_est_vide(l) == 1)
    {
        printf("la liste est vide !\n");
    }
    else if(l->premier->precedent == NULL && l->premier->id == id)
    {
        supprimer_debut(l);
    } 
    else {
        supprimer_val_recur(l -> premier, id);
    }
}


/**
 * @brief 
 * fonction auxiliaire de supprimer_fin qui parcours la liste
 * @param element Liste des clients contenue dans la structure "liste"
 * @param val Identifiant du client à supprimmer
 */
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



/**
 * @brief 
 * fonction qui affiche les différents éléments d'une liste
 * @param liste Le pointeur de la structure liste qui contient une liste de clients 
 */
void afficherListe(liste *liste)
{
    if (liste == NULL)
    {
        printf("La liste est vide\n");
    }
    Element * actuel = liste->premier;
    while (actuel->suivant != NULL)
    {
        printf("L'utilisateur %s (id : %d), il est connecté au channel : %s\n", actuel->pseudo, actuel->id, actuel->canal);
        actuel = actuel->suivant;
    }
}


/**
 * @brief 
 * foncton qui retourne l'élément suivant, si il y'en a pas, return l'élément 
 * @param l Le pointeur de la structure liste qui contient une liste de clients
 * @return Element* 
 */
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

/**
 * @brief 
 * fonction qui return l'élément precedent, si il y'en a pas, return l'élément 
 * @param l Le pointeur de la structure liste qui contient une liste de clients
 * @return Element* 
 */
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


/**
 * @brief 
 * retourne la taille de la liste
 * @param l Le pointeur de la structure liste qui contient une liste de clients
 * @return int 
 */
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