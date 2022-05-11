#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liste.h"


//fonction de création d'une liste
//Post : retourne une liste vide stockée à l'adresse de la variable "li"
liste * creer_liste(){
    liste * li = malloc(sizeof(liste));
    li -> suivant = NULL;
    li -> precedent = NULL;
    li -> id = 0;
    li -> pseudo = NULL; 
    return li;
}


//fonction de vérification si une liste est vide
//Pre : il faut que la liste existe
//Post : retourne 1 (true) si pour cette liste le premier élémen ne posséde pas d'identifiant
//donc bien vide. Retourne 0 sinon 
int liste_est_vide(liste * l){
    if((l -> id == 0 && l -> pseudo == NULL) || l == NULL){
        return 1;
    }
    else{
        return 0;
    }
}


//fonction pour ajouter au début d'une liste un élément
//Post : remplace la liste passée en paramètres par référence par la nouvelle liste  
void ajouter_debut(liste * l, int identifiant, char * chaine){
    liste * debut = malloc(sizeof(liste));
    debut -> suivant = l;
    debut -> id = identifiant;
    debut -> pseudo = chaine;
    debut -> precedent = NULL;
    l->precedent = debut;
}


//fonction pour supprimer l'élément du début de la liste
//Pre : Si la liste est vide, il n'y a rien à supprimer
//Post : retourne la liste en retirant l'élément de début de la liste
liste* supprimer_debut(liste * l){
    if(liste_est_vide(l)){
        perror("La liste est vide, on peut rien supprimer");
    }
    else{
        liste * nvl = (liste*) malloc(sizeof(liste));
        nvl = l -> suivant;
        nvl->precedent = NULL;
        //l = nvl;
        return nvl;
    }
}


//fonction pour ajouter un élément à la fin de la liste
//Pre : identifiant doit être un entier (socket) et char un pseudo
//Post : modifie la liste en ajoutant un élément à la fin
liste * ajouter_fin(liste * l, int identifiant, char * chaine){
    if(liste_est_vide(l)){
        liste * li = creer_liste();
        li->id = identifiant;
        li->pseudo = chaine;
        li->precedent = creer_liste();
        li->suivant = creer_liste();
        return li;
    }
    else{
        liste * li = malloc(sizeof(*li));
        li = l;
        printf("L'adresse du suivant est : %p\n", li->suivant);
        while(liste_est_vide(li -> suivant) == 0){
            //printf("Je suis dans le while \n");
            //printf("L'adresse du suivant est : %p\n", li->suivant);
            //printf("L'adresse du suivant du suivant est : %p\n", li->suivant->suivant);
            li = li->suivant;
            //printf("La nouvelle adresse de li est : %p \n",li);
        }
        //printf("Je suis sorti du while \n");
        liste * fin = creer_liste();
        //printf("La liste est vide ou pas %d\n",liste_est_vide(li->suivant));
        //printf("L'adresse suivante de la liste est : %p\n", li->suivant);
        fin->id = identifiant;
        fin->pseudo = chaine;
        fin -> precedent = li;
        //afficherListe(fin);
        //printf("J'arrive pas à concaténer\n");
        //li->suivant = NULL;
        li->suivant = fin;
        //printf("Tout s'est bien passé");
        return l;
    }
}


//fonction qui supprime le dernier élément d'une liste 
//Pre : la liste ne doit pas être vide
//Post : retourne la liste modifiée, elle ne possède plus son dernier élément
liste * supprimer_fin(liste * l){
    liste * li = malloc(sizeof(*li));
    li = l;
    if(liste_est_vide(li)){
        perror("La liste est vide, je ne peux rien supprimer \n");
    }
    else{
        if(liste_est_vide(li -> suivant)){
            return creer_liste();
        }
        else{
            while(liste_est_vide(li -> suivant -> suivant) == 0){
                li = li -> suivant;
            }
            li -> suivant = creer_liste();
            return li;
        }
    }
}


//fonction qui supprime la valeur passée en paramètre
//Pre : la liste ne doit pas être vide et elle doit contenir l'élément
//Post : la liste est modifiée si elle contient bien l'élément, une seule occurence est supprimée
void supprimer_val(liste * l, int val){
    if(liste_est_vide(l)){
        printf("la liste est vide\n");
        perror("L'élément n'a pas été trouvé\n");
    }
    else{
        if(l -> id == val){
            if(l->suivant != NULL && l->precedent != NULL){
                l->suivant->precedent = l->precedent;
                l->precedent->suivant = l-> suivant;
            }
            /*
            else if(l->precedent == NULL){
                l->suivant->precedent = NULL;
            }
            else if(l->suivant == NULL){
                l->precedent->suivant = NULL;
            }
            */
            liste * Asupprimer = l;
            free(Asupprimer);
        }
        else{
            supprimer_val(l->suivant, val);
        }
    }
}


//fonction qui donne la taille de la liste passée en paramètre
//Post : retourne la taille
int taille_liste(liste * l){
    
    liste * actuel = l;
    int taille = 0;
    while(l -> suivant != NULL)
    {
        taille++;
    }
    return taille;
    /*
    liste * li = malloc(sizeof(*li));
    li = l;
    int taille = 0;
    if(liste_est_vide(l) == 0){
        taille++;
        while(liste_est_vide(li -> suivant) == 0){
            taille ++;
            li = li -> suivant;
        }
    }
    */
    return taille;
}


//fonction qui parcourt la liste en affichant les éléments de la liste
//Post : ne retourne rien, affiche seulement à l'écran les éléments
void afficherListe(liste * l){
    printf("taille %d\n", taille_liste(l));
    while(l -> suivant != NULL){
        printf("L'identifiant est %d et le pseudo est : %s \n",l->id, l->pseudo);
        l = l->suivant;
    }
}



int main(){
    liste * li;
    li = creer_liste();
    int res = liste_est_vide(li);
    printf("Ladresse de li est : %p \n",li);
    printf("L'objet pointé par li est : %p \n", &(*li));
    if(res){
        printf("La liste est vide \n");
    }
    else{
        printf("La liste est pleine \n");
    }
    char pseudo[10] = "Salim";
    char pseudo2[10] = "ayoub";
    char pseudo3[10] = "Hamid";
    ajouter_debut(li, 1, pseudo);
    afficherListe(li);
    printf("La taille de la liste est : %d \n", taille_liste(li));
    ajouter_debut(li, 2, pseudo2);
    printf("La nouvelle taille de la liste est : %d \n", taille_liste(li));
    afficherListe(li);
    ajouter_debut(li,3,pseudo3);
    printf("La taille est : %d \n", taille_liste(li));
    afficherListe(li);
    supprimer_val(li, 2);
    printf("Après avoir supprimé, la taille est de maintenant : %d\n", taille_liste(li));
    afficherListe(li);
    return 0;
}