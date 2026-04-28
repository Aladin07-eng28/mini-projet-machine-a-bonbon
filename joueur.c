#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joueur.h"

joueur* creer_joueur(int id, const char *pseudo)
{
    joueur *j = malloc(sizeof(joueur));
    if (!j) return NULL;
    strncpy(j->pseudo, pseudo, TAILLE_PSEUDO - 1);
    j->pseudo[TAILLE_PSEUDO - 1] = '\0';
    j->id = id;
    j->score = 0;
    j->meilleur_score = 0;
    j->nb_parties = 0;
    j->score_temp = 0;
    return j;
}

void afficher_joueur(const joueur *j)
{
    if (!j) return;
    printf("%s : %d pts\n", j->pseudo, j->score);
}

void mettre_a_jour_score(joueur *j, int points)
{
    if (!j) return;
    j->score += points;
    if (j->score > j->meilleur_score) j->meilleur_score = j->score;
}

void supprimer_joueur(joueur *j)
{
    if (!j) return;
    free(j);
}


ListeJoueurs* creer_groupe_joueurs()
{
    ListeJoueurs *L = malloc(sizeof(ListeJoueurs));
    if (!L) return NULL;
    L->tete = L->queue = NULL;
    L->taille = 0;
    return L;
}

NoeudJoueur* ajouter_joueur_queue(ListeJoueurs *L, joueur *j)
{
    if (!L || !j) return NULL;
    NoeudJoueur *node = malloc(sizeof(NoeudJoueur));
    if (!node) return NULL;
    node->j = j;
    node->prev = L->queue;
    node->next = NULL;
    if (L->queue) L->queue->next = node;
    L->queue = node;
    if (!L->tete) L->tete = node;
    L->taille++;
    return node;
}

joueur* trouver_gagnant(const ListeJoueurs *L)
{
    if (!L || L->taille == 0) return NULL;
    NoeudJoueur *cur = L->tete;
    joueur *best = cur->j;
    cur = cur->next;
    while (cur) {
        if (cur->j->score > best->score) best = cur->j;
        cur = cur->next;
    }
    return best;
}

void liberer_groupe_joueurs(ListeJoueurs *L)
{
    if (!L) return;
    NoeudJoueur *cur = L->tete;
    while (cur) {
        NoeudJoueur *tmp = cur;
        cur = cur->next;
        supprimer_joueur(tmp->j);
        free(tmp);
    }
    free(L);
}
