#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bonbon.h"

Bonbon* creer_Bonbon(const char couleur[], const char type[], int id, int valeur)
{
    Bonbon *b = malloc(sizeof(Bonbon));
    if (!b) return NULL;

    strcpy(b->couleur, couleur);
    strcpy(b->type, type);
    b->id = id;
    b->valeur = valeur;
    b->suivant = NULL;

    return b;
}


void afficher_Bonbon(const Bonbon *b)
{
    printf("Bonbon: couleur=%s | type=%s | id=%d | valeur=%d\n",
           b->couleur, b->type, b->id, b->valeur);
}

void supprimer_Bonbon(Bonbon *b)
{
    free(b);
}
