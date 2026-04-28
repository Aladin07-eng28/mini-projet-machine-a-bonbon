#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "distributeur.h"

Distributeur* creerDistributeur(int id, int capacite)
{
    Distributeur *d = malloc(sizeof(Distributeur));
    if (!d) return NULL;
    d->id = id;
    d->capacite = capacite;
    d->nbBonbons = 0;
    d->premier = NULL;
    return d;
}

void libererDistributeurUnit(Distributeur *d)
{
    if (!d) return;
    Bonbon *b = d->premier;
    while (b) {
        Bonbon *tmp = b;
        b = b->suivant;
        supprimer_Bonbon(tmp);
    }
    free(d);
}

void ajouterBonbonDistributeur(Distributeur *d, Bonbon *b)
{
    if (!d || !b) return;
    if (d->nbBonbons >= d->capacite) {

        supprimer_Bonbon(b);
        return;
    }
    b->suivant = d->premier;
    d->premier = b;
    d->nbBonbons++;
}

Bonbon* retirerBonbonDistributeur(Distributeur *d)
{
    if (!d || d->premier == NULL) return NULL;
    Bonbon *b = d->premier;
    d->premier = b->suivant;
    b->suivant = NULL;
    d->nbBonbons--;
    return b;
}

void afficherDistributeur(const Distributeur *d)
{
    if (!d) return;
    printf("\n--- Distributeur %d ---\n", d->id);
    printf("Capacite : %d | Nb bonbons : %d\n", d->capacite, d->nbBonbons);
    Bonbon *b = d->premier;
    int i = 1;
    while (b) {
        printf(" %d) ", i++);
        afficher_Bonbon(b);
        b = b->suivant;
    }
}


int tailleDistributeur(const Distributeur *d)
{
    if (!d) return 0;
    return d->nbBonbons;
}



ListeDistributeur* creerListeDistributeur()
{
    ListeDistributeur *L = malloc(sizeof(ListeDistributeur));
    if (!L) return NULL;
    L->tete = L->queue = NULL;
    L->taille = 0;
    return L;
}

void libererListeDistributeur(ListeDistributeur *L)
{
    if (!L) return;
    NoeudDistributeur *cur = L->tete;
    while (cur) {
        NoeudDistributeur *tmp = cur;
        cur = cur->next;
        libererDistributeurUnit(tmp->d);
        free(tmp);
    }
    free(L);
}

NoeudDistributeur* ajouterDistributeurQueue(ListeDistributeur *L, Distributeur *d)
{
    if (!L || !d) return NULL;
    NoeudDistributeur *node = malloc(sizeof(NoeudDistributeur));
    if (!node) return NULL;
    node->d = d;
    node->prev = L->queue;
    node->next = NULL;
    if (L->queue) L->queue->next = node;
    L->queue = node;
    if (!L->tete) L->tete = node;
    L->taille++;
    return node;
}

int supprimerDistributeurParId(ListeDistributeur *L, int id)
{
    if (!L) return 0;
    NoeudDistributeur *cur = L->tete;
    while (cur) {
        if (cur->d->id == id) {
            if (cur->prev) cur->prev->next = cur->next;

            else L->tete = cur->next;

            if (cur->next) cur->next->prev = cur->prev;

            else L->queue = cur->prev;

            libererDistributeurUnit(cur->d);

            free(cur);

            L->taille--;

            return 1;
        }
        cur = cur->next;
    }
    return 0;
}


Distributeur* trouverDistributeurParIndex(const ListeDistributeur *L, int index)
{
    if (!L || index < 0 || index >= L->taille) return NULL;

    NoeudDistributeur *cur;
    int i;

    if (index < L->taille / 2) {

        cur = L->tete; i = 0;

        while (cur && i < index) { cur = cur->next; i++; }

    } else {
        cur = L->queue; i = L->taille - 1;

        while (cur && i > index) { cur = cur->prev; i--; }
    }
    return cur ? cur->d : NULL;
}

/* ===================== Persistance (fichiers) =====================
   Format (distributeurs.txt) :
   1ere ligne : N (nombre de distributeurs)
   Pour chaque distributeur :
     id;capacite;nbBonbons\n
     puis nbBonbons lignes :
     idBonbon;couleur;type;valeur\n
*/

int chargerDistributeurs(ListeDistributeur *L, const char *filename)
{
    if (!L || !filename) return 0;

    FILE *f = fopen(filename, "r");
    if (!f) return 0;

    char line[256];
    if (!fgets(line, sizeof(line), f)) { fclose(f); return 0; }

    int n = atoi(line);
    if (n <= 0) { fclose(f); return 0; }

    int nbD = 0;

    for (int i = 0; i < n; i++) {
        if (!fgets(line, sizeof(line), f)) break;

        int id = 0, cap = 0, nbB = 0;
        if (sscanf(line, "%d;%d;%d", &id, &cap, &nbB) != 3) break;

        Distributeur *d = creerDistributeur(id, cap);
        if (!d) break;

        for (int k = 0; k < nbB; k++) {
            if (!fgets(line, sizeof(line), f)) break;

            int bid = 0, val = 0;
            char couleur[TAILLE_COULEUR];
            char type[TAILLE_TYPE];

            /* couleur/type sans espaces (Rouge, Vert, epic, ...) */
            if (sscanf(line, "%d;%19[^;];%19[^;];%d", &bid, couleur, type, &val) != 4)
                continue;

            Bonbon *b = creer_Bonbon(couleur, type, bid, val);
            if (b) ajouterBonbonDistributeur(d, b);
        }

        ajouterDistributeurQueue(L, d);
        nbD++;
    }

    fclose(f);
    return nbD;
}

int sauvegarderDistributeurs(const ListeDistributeur *L, const char *filename)
{
    if (!L || !filename) return 0;

    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    fprintf(f, "%d\n", L->taille);

    int nbD = 0;
    for (NoeudDistributeur *cur = L->tete; cur; cur = cur->next) {
        Distributeur *d = cur->d;
        fprintf(f, "%d;%d;%d\n", d->id, d->capacite, d->nbBonbons);

        for (Bonbon *b = d->premier; b; b = b->suivant) {
            fprintf(f, "%d;%s;%s;%d\n", b->id, b->couleur, b->type, b->valeur);
        }
        nbD++;
    }

    fclose(f);
    return nbD;
}
