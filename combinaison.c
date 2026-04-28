#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "combinaison.h"

static int points_type(const char *type) {
    if (strcmp(type, "ordinaire") == 0) return 1;
    if (strcmp(type, "rare")      == 0) return 2;
    if (strcmp(type, "epic")      == 0) return 3;
    return 0;
}

static int bonus_couleurs(char couleurs[][TAILLE_COULEUR_COMB]) {
    int c01 = (strcmp(couleurs[0], couleurs[1]) == 0);
    int c02 = (strcmp(couleurs[0], couleurs[2]) == 0);
    int c12 = (strcmp(couleurs[1], couleurs[2]) == 0);

    if (c01 && c02)
        return 2;
    else
    if (c01 || c02 || c12)

        return 1;
    else
        return 0;
}

combinaison* creer_combinaison(int id, char couleurs[][TAILLE_COULEUR_COMB], char types[][TAILLE_TYPE_COMB])
{
    combinaison *c = malloc(sizeof(combinaison));
    if (!c) return NULL;
    c->id = id;
    int base_score = 0;
    for (int i = 0; i < MAX_ITEMS; i++)
        {
        strncpy(c->couleurs[i], couleurs[i], TAILLE_COULEUR_COMB - 1);

        c->couleurs[i][TAILLE_COULEUR_COMB - 1] = '\0';

        strncpy(c->types[i], types[i], TAILLE_TYPE_COMB - 1);

        c->types[i][TAILLE_TYPE_COMB - 1] = '\0';

        base_score += points_type(c->types[i]);
    }
    int bonus = bonus_couleurs(c->couleurs);

    c->points = base_score + bonus;

    return c;
}

void afficher_combinaison(const combinaison *c)
{
    if (!c) return;

    printf("======\n");

    printf("Combinaison id = %d\n", c->id);

    int base_score = 0;

    for (int i = 0; i < MAX_ITEMS; i++) {

        int pts = points_type(c->types[i]);

        base_score += pts;

        printf("  %d) couleur=%-8s | type=%-9s -> %d point(s)\n",

               i + 1, c->couleurs[i], c->types[i], pts);
    }
    int bonus = bonus_couleurs((char(*)[TAILLE_COULEUR_COMB])c->couleurs);

    printf("\nRegles de calcul des points :\n");
    printf(" - ordinaire = 1 point\n");
    printf(" - rare      = 2 points\n");
    printf(" - epic      = 3 points\n");
    printf(" - 3 memes couleurs : +2 points bonus\n");
    printf(" - 2 memes couleurs : +1 point bonus\n");

    printf("\nBase score    : %d\n", base_score);
    printf("Bonus couleurs: %d\n", bonus);
    printf("Points totaux : %d\n", c->points);
    printf("===\n");
}

void supprimer_combinaison(combinaison *c)
{
    if (!c) return;
    free(c);
}
int supprimer_combinaison_par_index(ListeCombinaison *LC, int index)
{
    if (!LC || index < 1 || index > LC->taille)
        return 0;

    NoeudCombinaison *cur = LC->tete;
    int i = 1;

    while (cur && i < index) {
        cur = cur->next;
        i++;
    }

    if (!cur)
        return 0;

    if (cur->prev)
        cur->prev->next = cur->next;
    else
        LC->tete = cur->next;

    if (cur->next)
        cur->next->prev = cur->prev;
    else
        LC->queue = cur->prev;

    supprimer_combinaison(cur->c);
    free(cur);

    LC->taille--;
    return 1;
}

int modifier_combinaison_par_index(ListeCombinaison *LC, int index)
{
    if (!LC || index < 1 || index > LC->taille)
        return 0;

    NoeudCombinaison *cur = LC->tete;
    int i = 1;

    while (cur && i < index) {
        cur = cur->next;
        i++;
    }

    if (!cur)
        return 0;


    char couleurs[MAX_ITEMS][TAILLE_COULEUR_COMB];
    char types[MAX_ITEMS][TAILLE_TYPE_COMB];

    for (int k = 0; k < MAX_ITEMS; k++) {
        printf("Nouvelle couleur %d : ", k + 1);
        fgets(couleurs[k], TAILLE_COULEUR_COMB, stdin);
        couleurs[k][strcspn(couleurs[k], "\n")] = '\0';

        printf("Nouveau type %d : ", k + 1);
        fgets(types[k], TAILLE_TYPE_COMB, stdin);
        types[k][strcspn(types[k], "\n")] = '\0';
    }

    supprimer_combinaison(cur->c);


    cur->c = creer_combinaison(index, couleurs, types);

    return 1;
}


ListeCombinaison* creerListeCombinaison()
{
    ListeCombinaison *L = malloc(sizeof(ListeCombinaison));

    if (!L) return NULL;
    L->tete = L->queue = NULL;
    L->taille = 0;
    return L;
}

void libererListeCombinaison(ListeCombinaison *L)
{
    if (!L) return;
    NoeudCombinaison *cur = L->tete;
    while (cur) {
        NoeudCombinaison *tmp = cur;
        cur = cur->next;
        supprimer_combinaison(tmp->c);
        free(tmp);
    }
    free(L);
}

NoeudCombinaison* ajouterCombinaisonQueue(ListeCombinaison *L, combinaison *c)
{
    if (!L || !c) return NULL;
    NoeudCombinaison *node = malloc(sizeof(NoeudCombinaison));
    if (!node) return NULL;
    node->c = c;
    node->prev = L->queue;
    node->next = NULL;
    if (L->queue) L->queue->next = node;
    L->queue = node;
    if (!L->tete) L->tete = node;
    L->taille++;
    return node;
}

/* ===================== Persistance (fichiers) =====================
   Format (combinaisons.txt) :
   id;col1;typ1;col2;typ2;col3;typ3\n
   Les points sont recalcules automatiquement via creer_combinaison().
*/

int chargerCombinaisons(ListeCombinaison *L, const char *filename)
{
    if (!L || !filename) return 0;

    FILE *f = fopen(filename, "r");
    if (!f) return 0;

    char line[256];
    int nb = 0;

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\n' || line[0] == '\0') continue;

        char *p[7];
        int k = 0;
        char *tok = strtok(line, ";\n");
        while (tok && k < 7) {
            p[k++] = tok;
            tok = strtok(NULL, ";\n");
        }
        if (k != 7) continue;

        int id = atoi(p[0]);
        char col[MAX_ITEMS][TAILLE_COULEUR_COMB];
        char typ[MAX_ITEMS][TAILLE_TYPE_COMB];

        strncpy(col[0], p[1], TAILLE_COULEUR_COMB - 1); col[0][TAILLE_COULEUR_COMB - 1] = '\0';
        strncpy(typ[0], p[2], TAILLE_TYPE_COMB - 1);    typ[0][TAILLE_TYPE_COMB - 1] = '\0';
        strncpy(col[1], p[3], TAILLE_COULEUR_COMB - 1); col[1][TAILLE_COULEUR_COMB - 1] = '\0';
        strncpy(typ[1], p[4], TAILLE_TYPE_COMB - 1);    typ[1][TAILLE_TYPE_COMB - 1] = '\0';
        strncpy(col[2], p[5], TAILLE_COULEUR_COMB - 1); col[2][TAILLE_COULEUR_COMB - 1] = '\0';
        strncpy(typ[2], p[6], TAILLE_TYPE_COMB - 1);    typ[2][TAILLE_TYPE_COMB - 1] = '\0';

        combinaison *c = creer_combinaison(id, col, typ);
        if (c) {
            ajouterCombinaisonQueue(L, c);
            nb++;
        }
    }

    fclose(f);
    return nb;
}

int sauvegarderCombinaisons(const ListeCombinaison *L, const char *filename)
{
    if (!L || !filename) return 0;

    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    int nb = 0;
    for (NoeudCombinaison *cur = L->tete; cur; cur = cur->next) {
        combinaison *c = cur->c;
        fprintf(f, "%d;%s;%s;%s;%s;%s;%s\n",
                c->id,
                c->couleurs[0], c->types[0],
                c->couleurs[1], c->types[1],
                c->couleurs[2], c->types[2]);
        nb++;
    }

    fclose(f);
    return nb;
}
