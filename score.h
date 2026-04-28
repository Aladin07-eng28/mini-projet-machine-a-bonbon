
#ifndef SCORE_H_INCLUDED
#define SCORE_H_INCLUDED

#define TAILLE_DATE   20
#define TAILLE_PSEUDO 30

typedef struct Score {
    char pseudo[TAILLE_PSEUDO];
    int points;
    char date[TAILLE_DATE];
} Score;


typedef struct NoeudScore {
    Score s;
    struct NoeudScore *prev;
    struct NoeudScore *next;
} NoeudScore;

typedef struct ListeScore {
    NoeudScore *tete;
    NoeudScore *queue;
    int taille;
} ListeScore;


ListeScore* creerListeScore();
void libererListeScore(ListeScore *L);
void ajouterScore(ListeScore *L, const char pseudo[], int points, const char date[]);
void afficherScores(const ListeScore *L);
void trierScoresDesc(ListeScore *L);
void supprimerScoreParPseudo(ListeScore *L, const char pseudo[]);
void obtenirDateActuelle(char date[]);

/* Persistance (fichiers) */
int chargerScores(ListeScore *L, const char *filename);
int sauvegarderScores(const ListeScore *L, const char *filename);

/* Statistiques (Option A) basees uniquement sur scores.txt */
void afficherStatistiquesScores(const ListeScore *L);

#endif
