#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "score.h"

ListeScore* creerListeScore()
{
    ListeScore *L = malloc(sizeof(ListeScore));
    if (!L) return NULL;
    L->tete = L->queue = NULL;
    L->taille = 0;
    return L;
}

void libererListeScore(ListeScore *L)
{
    if (!L) return;
    NoeudScore *cur = L->tete;
    while (cur) {
        NoeudScore *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    free(L);
}

void ajouterScore(ListeScore *L, const char pseudo[], int points, const char date[])
{
    if (!L || !pseudo) return;
    NoeudScore *node = malloc(sizeof(NoeudScore));

    if (!node) return;
    strncpy(node->s.pseudo, pseudo, TAILLE_PSEUDO - 1);

    node->s.pseudo[TAILLE_PSEUDO - 1] = '\0';

    node->s.points = points;

    strncpy(node->s.date, date, TAILLE_DATE - 1);

    node->s.date[TAILLE_DATE - 1] = '\0';

    node->prev = L->queue;

    node->next = NULL;

    if (L->queue) L->queue->next = node;

    L->queue = node;

    if (!L->tete) L->tete = node;
    L->taille++;
}

void afficherScores(const ListeScore *L)
{
    if (!L || L->taille == 0) {
        printf("(aucun score)\n");
        return;
    }
    NoeudScore *cur = L->tete;
    int idx = 1;
    while (cur) {
        printf("%d) %s - %d pts - %s\n", idx++, cur->s.pseudo, cur->s.points, cur->s.date);
        cur = cur->next;
    }
}

void trierScoresDesc(ListeScore *L)
{
    if (!L || L->taille < 2) return;

    NoeudScore *sorted = NULL;
    NoeudScore *cur = L->tete;
    while (cur) {
        NoeudScore *next = cur->next;

        cur->prev = cur->next = NULL;
        if (!sorted) {

            sorted = cur;

            sorted->prev = sorted->next = NULL;
        } else {

            NoeudScore *s = sorted;

            NoeudScore *insert_before = NULL;
            while (s) {
                if (cur->s.points > s->s.points) { insert_before = s; break; }
                s = s->next;
            }
            if (!insert_before) {
                /* append end */
                NoeudScore *tail = sorted;
                while (tail->next) tail = tail->next;
                tail->next = cur;
                cur->prev = tail;
            } else {

                cur->next = insert_before;

                cur->prev = insert_before->prev;

                insert_before->prev = cur;

                if (cur->prev) cur->prev->next = cur;

                else sorted = cur;
            }
        }
        cur = next;
    }

    NoeudScore *tail = sorted;

    NoeudScore *head = sorted;

    if (tail) { while (tail->next) tail = tail->next; while (head->prev) head = head->prev; }

    L->tete = head;

    L->queue = tail;
}

void supprimerScoreParPseudo(ListeScore *L, const char pseudo[])
{
    if (!L || !pseudo) return;

    NoeudScore *cur = L->tete;

    while (cur) {

        NoeudScore *next = cur->next;

        if (strcmp(cur->s.pseudo, pseudo) == 0) {

            if (cur->prev) cur->prev->next = cur->next;

            else L->tete = cur->next;

            if (cur->next) cur->next->prev = cur->prev;

            else L->queue = cur->prev;

            free(cur);

            L->taille--;

        }
        cur = next;
    }
}

void obtenirDateActuelle(char date[])
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    sprintf(date, "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
}

/* ===================== Persistance (fichiers) =====================
   Format (scores.txt) :
   pseudo;points;date\n
   Exemple :
   Ahmed;12;18/12/2025
*/

int chargerScores(ListeScore *L, const char *filename)
{
    if (!L || !filename) return 0;

    FILE *f = fopen(filename, "r");
    if (!f) return 0; /* fichier inexistant -> ok */

    char line[256];
    int nb = 0;

    while (fgets(line, sizeof(line), f)) {
        /* ignore lignes vides */
        if (line[0] == '\n' || line[0] == '\0') continue;

        char *p1 = strtok(line, ";\n");
        char *p2 = strtok(NULL, ";\n");
        char *p3 = strtok(NULL, ";\n");
        if (!p1 || !p2 || !p3) continue;

        int points = atoi(p2);
        ajouterScore(L, p1, points, p3);
        nb++;
    }

    fclose(f);
    return nb;
}

int sauvegarderScores(const ListeScore *L, const char *filename)
{
    if (!L || !filename) return 0;

    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    int nb = 0;
    for (NoeudScore *cur = L->tete; cur; cur = cur->next) {
        fprintf(f, "%s;%d;%s\n", cur->s.pseudo, cur->s.points, cur->s.date);
        nb++;
    }

    fclose(f);
    return nb;
}

/* ===================== Statistiques (Option A) =====================
   IMPORTANT: on n'ajoute PAS de nouveau fichier (pas de stats.c/h).
   Les statistiques sont calculees uniquement a partir de la liste des
   scores (chargee depuis scores.txt).
*/

typedef struct {
    char pseudo[TAILLE_PSEUDO];
    long nb_entrees;
    long total_points;
    int best_points;
    char best_date[TAILLE_DATE];
} PlayerStat;

static int str_ieq(const char *a, const char *b)
{
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

static int find_player(PlayerStat *arr, int n, const char *pseudo)
{
    for (int i = 0; i < n; i++)
        if (str_ieq(arr[i].pseudo, pseudo)) return i;
    return -1;
}

static int cmp_best_desc(const void *pa, const void *pb)
{
    const PlayerStat *a = (const PlayerStat*)pa;
    const PlayerStat *b = (const PlayerStat*)pb;

    if (b->best_points != a->best_points) return b->best_points - a->best_points;

    if (b->total_points != a->total_points)
        return (b->total_points > a->total_points) ? 1 : -1;

    return strcmp(a->pseudo, b->pseudo);
}

static int cmp_avg_desc(const void *pa, const void *pb)
{
    const PlayerStat *a = (const PlayerStat*)pa;
    const PlayerStat *b = (const PlayerStat*)pb;

    double avga = a->nb_entrees ? (double)a->total_points / (double)a->nb_entrees : 0.0;
    double avgb = b->nb_entrees ? (double)b->total_points / (double)b->nb_entrees : 0.0;

    if (avgb > avga) return 1;
    if (avgb < avga) return -1;

    if (b->best_points != a->best_points) return b->best_points - a->best_points;

    return strcmp(a->pseudo, b->pseudo);
}

void afficherStatistiquesScores(const ListeScore *L)
{
    if (!L || L->taille == 0) {
        printf("\n[STATS] Aucun score enregistre.\n");
        return;
    }

    long nb_entrees = L->taille;
    long total_points = 0;

    int best_points = -2147483647;
    char best_pseudo[TAILLE_PSEUDO] = "";
    char best_date[TAILLE_DATE] = "";

    PlayerStat *players = (PlayerStat*)calloc((size_t)L->taille, sizeof(PlayerStat));
    if (!players) {
        printf("\n[STATS] Memoire insuffisante.\n");
        return;
    }
    int nplayers = 0;

    for (NoeudScore *cur = L->tete; cur; cur = cur->next) {
        total_points += cur->s.points;

        if (cur->s.points > best_points) {
            best_points = cur->s.points;
            strncpy(best_pseudo, cur->s.pseudo, TAILLE_PSEUDO - 1);
            best_pseudo[TAILLE_PSEUDO - 1] = '\0';
            strncpy(best_date, cur->s.date, TAILLE_DATE - 1);
            best_date[TAILLE_DATE - 1] = '\0';
        }

        int idx = find_player(players, nplayers, cur->s.pseudo);
        if (idx < 0) {
            idx = nplayers++;
            strncpy(players[idx].pseudo, cur->s.pseudo, TAILLE_PSEUDO - 1);
            players[idx].pseudo[TAILLE_PSEUDO - 1] = '\0';
            players[idx].best_points = -2147483647;
            players[idx].best_date[0] = '\0';
        }

        players[idx].nb_entrees++;
        players[idx].total_points += cur->s.points;

        if (cur->s.points > players[idx].best_points) {
            players[idx].best_points = cur->s.points;
            strncpy(players[idx].best_date, cur->s.date, TAILLE_DATE - 1);
            players[idx].best_date[TAILLE_DATE - 1] = '\0';
        }
    }

    double moyenne = (double)total_points / (double)nb_entrees;

    printf("\n===== STATISTIQUES (Option A : scores.txt) =====\n");
    printf("Entrees de score        : %ld\n", nb_entrees);
    printf("Joueurs distincts       : %d\n", nplayers);
    printf("Points totaux           : %ld\n", total_points);
    printf("Moyenne (pts/entree)    : %.2f\n", moyenne);
    printf("Meilleur score global   : %s - %d pts - %s\n", best_pseudo, best_points, best_date);

    PlayerStat *tmp = (PlayerStat*)malloc((size_t)nplayers * sizeof(PlayerStat));
    if (tmp) {
        memcpy(tmp, players, (size_t)nplayers * sizeof(PlayerStat));

        qsort(tmp, (size_t)nplayers, sizeof(PlayerStat), cmp_best_desc);

        printf("\n--- TOP 5 (meilleur score) ---\n");
        int top = (nplayers < 5) ? nplayers : 5;
        for (int i = 0; i < top; i++) {
            double moy = tmp[i].nb_entrees ? (double)tmp[i].total_points / (double)tmp[i].nb_entrees : 0.0;
            printf("%d) %-15s | best=%d (%s) | entrees=%ld | total=%ld | moy=%.2f\n",
                   i + 1, tmp[i].pseudo, tmp[i].best_points, tmp[i].best_date,
                   tmp[i].nb_entrees, tmp[i].total_points, moy);
        }

        qsort(tmp, (size_t)nplayers, sizeof(PlayerStat), cmp_avg_desc);
        printf("\n--- TOP 5 (moyenne) ---\n");
        for (int i = 0; i < top; i++) {
            double moy = tmp[i].nb_entrees ? (double)tmp[i].total_points / (double)tmp[i].nb_entrees : 0.0;
            printf("%d) %-15s | moy=%.2f | entrees=%ld | total=%ld | best=%d (%s)\n",
                   i + 1, tmp[i].pseudo, moy, tmp[i].nb_entrees, tmp[i].total_points,
                   tmp[i].best_points, tmp[i].best_date);
        }
        free(tmp);
    }

    printf("\n--- Details par joueur ---\n");
    for (int i = 0; i < nplayers; i++) {
        double moy = players[i].nb_entrees ? (double)players[i].total_points / (double)players[i].nb_entrees : 0.0;
        printf("%-15s | entrees=%ld | total=%ld | moy=%.2f | best=%d (%s)\n",
               players[i].pseudo, players[i].nb_entrees, players[i].total_points,
               moy, players[i].best_points, players[i].best_date);
    }

    printf("===============================================\n");

    free(players);
}
