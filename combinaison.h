
#ifndef COMBINAISON_H_INCLUDED
#define COMBINAISON_H_INCLUDED

#define MAX_ITEMS 3
#define TAILLE_COULEUR_COMB 10
#define TAILLE_TYPE_COMB 10

typedef struct combinaison {
    int id;
    char couleurs[MAX_ITEMS][TAILLE_COULEUR_COMB];
    char types[MAX_ITEMS][TAILLE_TYPE_COMB];
    int points;
} combinaison;


typedef struct NoeudCombinaison {
    combinaison *c;
    struct NoeudCombinaison *prev;
    struct NoeudCombinaison *next;
} NoeudCombinaison;

typedef struct ListeCombinaison {
    NoeudCombinaison *tete;
    NoeudCombinaison *queue;
    int taille;
} ListeCombinaison;


combinaison* creer_combinaison(int id, char couleurs[][TAILLE_COULEUR_COMB], char types[][TAILLE_TYPE_COMB]);

void afficher_combinaison(const combinaison *c);

void supprimer_combinaison(combinaison *c);

int modifier_combinaison_par_index(ListeCombinaison *LC, int index);

int supprimer_combinaison_par_index(ListeCombinaison *LC, int index);



ListeCombinaison* creerListeCombinaison();
void libererListeCombinaison(ListeCombinaison *L);
NoeudCombinaison* ajouterCombinaisonQueue(ListeCombinaison *L, combinaison *c);

/* Persistance (fichiers) */
int chargerCombinaisons(ListeCombinaison *L, const char *filename);
int sauvegarderCombinaisons(const ListeCombinaison *L, const char *filename);

#endif
