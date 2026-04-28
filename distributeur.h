#ifndef DISTRIBUTEUR_H_INCLUDED
#define DISTRIBUTEUR_H_INCLUDED

#include "bonbon.h"


typedef struct Distributeur {
    int id;
    int capacite;
    int nbBonbons;
    Bonbon *premier;
} Distributeur;


typedef struct NoeudDistributeur {
    Distributeur *d;
    struct NoeudDistributeur *prev;
    struct NoeudDistributeur *next;
} NoeudDistributeur;

typedef struct ListeDistributeur {
    NoeudDistributeur *tete;
    NoeudDistributeur *queue;
    int taille;
} ListeDistributeur;


Distributeur* creerDistributeur(int id, int capacite);

void libererDistributeurUnit(Distributeur *d);

void ajouterBonbonDistributeur(Distributeur *d, Bonbon *b);

Bonbon* retirerBonbonDistributeur(Distributeur *d);

void afficherDistributeur(const Distributeur *d);

int tailleDistributeur(const Distributeur *d);

ListeDistributeur* creerListeDistributeur();

void libererListeDistributeur(ListeDistributeur *L);

NoeudDistributeur* ajouterDistributeurQueue(ListeDistributeur *L, Distributeur *d);

int supprimerDistributeurParId(ListeDistributeur *L, int id);

Distributeur* trouverDistributeurParIndex(const ListeDistributeur *L, int index);

/* Persistance (fichiers) */
int chargerDistributeurs(ListeDistributeur *L, const char *filename);
int sauvegarderDistributeurs(const ListeDistributeur *L, const char *filename);

#endif
