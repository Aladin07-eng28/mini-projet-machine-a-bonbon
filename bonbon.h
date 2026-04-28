#ifndef BONBON_H_INCLUDED
#define BONBON_H_INCLUDED

#define TAILLE_COULEUR 20
#define TAILLE_TYPE    20

typedef struct Bonbon {
    char couleur[TAILLE_COULEUR];
    char type[TAILLE_TYPE];
    int id;
    int valeur;
    struct Bonbon *suivant;
} Bonbon;

Bonbon* creer_Bonbon(const char couleur[], const char type[], int id, int valeur);

void afficher_Bonbon(const Bonbon *b);

void supprimer_Bonbon(Bonbon *b);

#endif
