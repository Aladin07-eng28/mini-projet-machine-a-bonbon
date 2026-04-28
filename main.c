#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bonbon.h"
#include "combinaison.h"
#include "distributeur.h"
#include "joueur.h"
#include "score.h"

#define TIRS_PAR_MANCHE 3

/* Fichiers de sauvegarde */
#define FICHIER_DISTRIBUTEURS "distributeurs.txt"
#define FICHIER_COMBINAISONS  "combinaisons.txt"
#define FICHIER_SCORES        "scores.txt"



int lireEntier() {
    char buf[32];
    fgets(buf, sizeof(buf), stdin);
    return atoi(buf);
}

void lireChaine(char *dest, int taille) {
    fgets(dest, taille, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}


void menu_principal() {
    printf("\n===== MACHINE A BONBONS =====\n");
    printf("1. Jouer une partie\n");
    printf("2. Parametrer le jeu\n");
    printf("3. Voir les scores\n");
    printf("4. Quitter\n");
    printf("5. Statistiques\n");
    printf("Votre choix : ");
}

void menu_parametrer_jeu() {
    printf("\n===== PARAMETRER LE JEU =====\n");
    printf("1. Parametrer machine\n");
    printf("2. Parametrer distributeurs\n");
    printf("3. Parametrer combinaisons\n");
    printf("4. Retour\n");
    printf("Votre choix : ");
}

void menu_param_distributeur() {
    printf("\n===== PARAMETRER DISTRIBUTEURS =====\n");
    printf("1. Ajouter distributeur\n");
    printf("2. Supprimer distributeur\n");
    printf("3. Modifier distributeur\n");
    printf("4. Afficher distributeurs\n");
    printf("5. Retour\n");
    printf("Votre choix : ");
}

void menu_param_combinaison() {
    printf("\n===== PARAMETRER COMBINAISONS =====\n");
    printf("1. Ajouter combinaison\n");
    printf("2. Supprimer combinaison\n");
    printf("3. Modifier combinaison\n");
    printf("4. Afficher combinaisons\n");
    printf("5. Retour\n");
    printf("Votre choix : ");
}



void remplir_distributeur_exemple(Distributeur *d) {
    const char *C[] = {"Rouge", "Vert", "Bleu"};
    const char *T[] = {"ordinaire", "rare", "epic"};
    int id = 1;

    while (tailleDistributeur(d) < d->capacite && id <= 20) {
        const char *co = C[rand() % 3];
        const char *ty = T[rand() % 3];

        int val = (strcmp(ty, "rare") == 0) ? 3 :
                  (strcmp(ty, "epic") == 0) ? 5 : 1;

        ajouterBonbonDistributeur(d, creer_Bonbon(co, ty, id++, val));
    }
}



combinaison* tirer_combinaison(ListeDistributeur *LD, int *idc)
{
    if (LD->taille < MAX_ITEMS) {
        printf("Pas assez de distributeurs.\n");
        return NULL;
    }

    char col[MAX_ITEMS][TAILLE_COULEUR_COMB];
    char typ[MAX_ITEMS][TAILLE_TYPE_COMB];

    NoeudDistributeur *cur = LD->tete;

    for (int i = 0; i < MAX_ITEMS; i++) {
        Bonbon *b = retirerBonbonDistributeur(cur->d);
        if (!b) {
            printf("Un distributeur est vide.\n");
            return NULL;
        }

        strcpy(col[i], b->couleur);
        strcpy(typ[i], b->type);
        supprimer_Bonbon(b);

        cur = cur->next;
    }

    return creer_combinaison((*idc)++, col, typ);
}

void reset_scores(ListeJoueurs *L) {
    for (NoeudJoueur *c = L->tete; c; c = c->next) {
        c->j->score = 0;
        c->j->score_temp = 0;
    }
}


void jouer_partie(ListeDistributeur *LD, ListeCombinaison *LC, ListeScore *LS) {
    printf("\n=== NOUVELLE PARTIE ===\n");

    printf("Nombre de joueurs : ");
    int nbJ = lireEntier();

    printf("Nombre de manches : ");
    int nbM = lireEntier();

    ListeJoueurs *L = creer_groupe_joueurs();

    for (int i = 0; i < nbJ; i++) {
        char pseudo[50];
        printf("Pseudo joueur %d : ", i + 1);
        lireChaine(pseudo, sizeof(pseudo));
        ajouter_joueur_queue(L, creer_joueur(i + 1, pseudo));
    }

    reset_scores(L);

    /* Pour eviter les doublons d'ID si on a charge des combinaisons depuis un fichier */
    int idc = 1;
    for (NoeudCombinaison *c = LC->tete; c; c = c->next)
        if (c->c && c->c->id >= idc) idc = c->c->id + 1;
    char input[10];

    for (int m = 1; m <= nbM; m++) {
        printf("\n===== MANCHE %d =====\n", m);

        int *scoresManche = calloc(nbJ, sizeof(int));

        for (int t = 1; t <= TIRS_PAR_MANCHE; t++) {
            printf("\n--- Tir %d ---\n", t);

            NoeudJoueur *curJ = L->tete;
            int idx = 0;

            while (curJ) {
                printf("(%s) Appuyez sur 't' : ", curJ->j->pseudo);
                do lireChaine(input, sizeof(input));
                while (strcmp(input, "t") && strcmp(input, "T"));

                combinaison *c = tirer_combinaison(LD, &idc);
                if (!c) return;

                afficher_combinaison(c);
                printf("Points : %d\n", c->points);

                scoresManche[idx] += c->points;
                mettre_a_jour_score(curJ->j, c->points);

                ajouterCombinaisonQueue(LC, c);

                idx++;
                curJ = curJ->next;
            }
        }

        int winnerIdx = 0;

        for (int i = 1; i < nbJ; i++)

            if (scoresManche[i] > scoresManche[winnerIdx])

                winnerIdx = i;

        NoeudJoueur *cj = L->tete;

        for (int k = 0; k < winnerIdx; k++) cj = cj->next;

        printf("\n*** Gagnant manche %d : %s (%d pts) ***\n",

               m, cj->j->pseudo, scoresManche[winnerIdx]);

        free(scoresManche);
    }

    joueur *GF = trouver_gagnant(L);
    printf("\n=== GAGNANT FINAL : %s (%d pts) ===\n", GF->pseudo, GF->score);

    char date[TAILLE_DATE];

    obtenirDateActuelle(date);

    for (NoeudJoueur *c = L->tete; c; c = c->next)
        ajouterScore(LS, c->j->pseudo, c->j->score, date);

   liberer_groupe_joueurs(L);

}



void afficher_menu_scores(ListeScore *LS) {
    if (!LS->taille) {
        printf("Aucun score.\n");
        return;
    }

    printf("\n--- Scores ---\n");
    afficherScores(LS);

    trierScoresDesc(LS);
    printf("\n--- Scores tries ---\n");
    afficherScores(LS);
}



void reindexer(ListeDistributeur *LD) {
    int i = 1;
    for (NoeudDistributeur *c = LD->tete; c; c = c->next)
        c->d->id = i++;
}

void menu_distributeurs(ListeDistributeur *LD) {
    int c;
    do {
        menu_param_distributeur();
        c = lireEntier();

        switch (c) {
        case 1: {
            printf("Capacite : ");

            int cap = lireEntier();

            Distributeur *d = creerDistributeur(LD->taille + 1, cap);

            remplir_distributeur_exemple(d);

            ajouterDistributeurQueue(LD, d);
            break;
        }

        case 2: {
            if (!LD->taille) { printf("Aucun.\n"); break; }
            printf("ID a supprimer : ");
            int id = lireEntier();
            if (supprimerDistributeurParId(LD, id))
                reindexer(LD);
            break;
        }

        case 3: {
            printf("ID : ");
            int id = lireEntier();
            Distributeur *d = trouverDistributeurParIndex(LD, id - 1);
            if (d) {
                printf("Nouvelle capacite : ");
                d->capacite = lireEntier();
            }
            break;
        }

        case 4:
            for (NoeudDistributeur *c = LD->tete; c; c = c->next)
                afficherDistributeur(c->d);
            break;
        }

    } while (c != 5);
}

void menu_combinaisons(ListeCombinaison *LC) {
    int c;

    do {
        menu_param_combinaison();
        c = lireEntier();

        switch (c) {

        case 1: {
            char col[MAX_ITEMS][TAILLE_COULEUR_COMB];
            char typ[MAX_ITEMS][TAILLE_TYPE_COMB];

            for (int i = 0; i < MAX_ITEMS; i++) {
                printf("Couleur %d : ", i + 1);
                lireChaine(col[i], TAILLE_COULEUR_COMB);

                printf("Type %d : ", i + 1);

                lireChaine(typ[i], TAILLE_TYPE_COMB);

            }

            ajouterCombinaisonQueue(
                LC,
                creer_combinaison(LC->taille + 1, col, typ)
            );
            break;
        }

        case 2: {
            printf("ID a supprimer : ");
            int id = lireEntier();
            supprimer_combinaison_par_index(LC, id);
            break;
        }

        case 3: {
            printf("ID a modifier : ");
            int id = lireEntier();
            modifier_combinaison_par_index(LC, id);
            break;
        }

        case 4:
            for (NoeudCombinaison *c = LC->tete; c; c = c->next)
                afficher_combinaison(c->c);
            break;
        }

    } while (c != 5);
}

void menu_machine(ListeDistributeur *LD) {
    int c;
    do {
        printf("\n===== PARAMETRER MACHINE =====\n");
        printf("1. Nombre de distributeurs\n");
        printf("2. Capacite distributeur\n");
        printf("3. Retour\n");
        printf("Votre choix : ");
        c = lireEntier();

        switch (c) {

        case 1: {
            printf("Nouveau nombre : ");
            int nv = lireEntier();
            while (LD->taille > nv)
                supprimerDistributeurParId(LD, LD->queue->d->id);
            while (LD->taille < nv) {
                Distributeur *d = creerDistributeur(LD->taille + 1, 200);
                remplir_distributeur_exemple(d);
                ajouterDistributeurQueue(LD, d);
            }
            break;
        }

        case 2: {
            printf("ID : ");
            int id = lireEntier();
            Distributeur *d = trouverDistributeurParIndex(LD, id - 1);
            if (d) {
                printf("Nouvelle capacite : ");
                d->capacite = lireEntier();
            }
            break;
        }
        }

    } while (c != 3);
}

void menu_parametrer(ListeDistributeur *LD, ListeCombinaison *LC) {
    int c;
    do {
        menu_parametrer_jeu();
        c = lireEntier();

        switch (c) {
        case 1: menu_machine(LD); break;
        case 2: menu_distributeurs(LD); break;
        case 3: menu_combinaisons(LC); break;
        }
    } while (c != 4);
}



int main() {
    srand(time(NULL));

    ListeDistributeur *LD = creerListeDistributeur();
    ListeCombinaison  *LC = creerListeCombinaison();
    ListeScore        *LS = creerListeScore();

    /* ===== Chargement depuis fichiers (si disponibles) ===== */
    int nd = chargerDistributeurs(LD, FICHIER_DISTRIBUTEURS);
    int nc = chargerCombinaisons(LC, FICHIER_COMBINAISONS);
    int ns = chargerScores(LS, FICHIER_SCORES);

    
     for (NoeudDistributeur *c = LD->tete; c; c = c->next) {
        if (tailleDistributeur(c->d) == 0) {
            remplir_distributeur_exemple(c->d);
        }
    }

    if (nd == 0) {
        /* Pas de fichier -> on initialise une configuration par defaut */
        for (int i = 1; i <= 3; i++) {
            Distributeur *d = creerDistributeur(i, 200);
            remplir_distributeur_exemple(d);
            ajouterDistributeurQueue(LD, d);
        }
    }

    printf("\n[INFO] Chargement: distributeurs=%d, combinaisons=%d, scores=%d\n",
           nd, nc, ns);

    int choix;
    do {
        menu_principal();
        choix = lireEntier();

        switch (choix) {
        case 1: jouer_partie(LD, LC, LS); break;
        case 2: menu_parametrer(LD, LC); break;
        case 3: afficher_menu_scores(LS); break;
        case 4: printf("Au revoir !\n"); break;
        case 5: afficherStatistiquesScores(LS); break;
        default: printf("Choix invalide.\n");
        }

    } while (choix != 4);

    /* ===== Sauvegarde dans fichiers ===== */
    sauvegarderDistributeurs(LD, FICHIER_DISTRIBUTEURS);
    sauvegarderCombinaisons(LC, FICHIER_COMBINAISONS);
    sauvegarderScores(LS, FICHIER_SCORES);

    libererListeDistributeur(LD);
    libererListeCombinaison(LC);
    libererListeScore(LS);

    return 0;
}
