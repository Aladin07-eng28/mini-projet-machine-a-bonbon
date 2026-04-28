#ifndef JOUEUR_H_INCLUDED
#define JOUEUR_H_INCLUDED

#define TAILLE_PSEUDO 30

typedef struct joueur {
    char pseudo[TAILLE_PSEUDO];
    int id;
    int score;
    int meilleur_score;
    int nb_parties;
    int score_temp;
} joueur;


typedef struct NoeudJoueur {
    joueur *j;
    struct NoeudJoueur *prev;
    struct NoeudJoueur *next;
} NoeudJoueur;

typedef struct ListeJoueurs {
    NoeudJoueur *tete;
    NoeudJoueur *queue;
    int taille;
} ListeJoueurs;

joueur* creer_joueur(int id, const char *pseudo);

void afficher_joueur(const joueur *j);

void mettre_a_jour_score(joueur *j, int points);

void supprimer_joueur(joueur *j);


ListeJoueurs* creer_groupe_joueurs();

void liberer_groupe_joueurs(ListeJoueurs *L);

NoeudJoueur* ajouter_joueur_queue(ListeJoueurs *L, joueur *j);

joueur* trouver_gagnant(const ListeJoueurs *L);


#endif
