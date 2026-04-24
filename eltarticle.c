#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ELTPRIM.H"

/* ---- viderBuffer : nettoie le buffer d'entree ---- */
static void viderBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ---- elementCreer : alloue un article vide ---- */
ELEMENT elementCreer(void) {
    ELEMENT e = (ELEMENT)malloc(sizeof(articleStruct));
    if (!e) printf("\nPlus d'espace memoire");
    return e;
}

/* ---- elementDetruire : libere la memoire ---- */
void elementDetruire(ELEMENT e) {
    if (e) free(e);
}

/* ---- elementAffecter : e1 pointe sur le meme objet que e2 ---- */
void elementAffecter(ELEMENT *e1, ELEMENT e2) {
    *e1 = e2;
}

/* ---- elementCopier : copie profonde du contenu de e2 dans *e1 ---- */
void elementCopier(ELEMENT *e1, ELEMENT e2) {
    if (!e1 || !e2) return;
    (*e1)->id             = e2->id;
    strncpy((*e1)->titre,  e2->titre,  99);  (*e1)->titre[99]  = '\0';
    strncpy((*e1)->source, e2->source, 49);  (*e1)->source[49] = '\0';
    (*e1)->score_fiabilite = e2->score_fiabilite;
    (*e1)->jour   = e2->jour;
    (*e1)->mois   = e2->mois;
    (*e1)->annee  = e2->annee;
    (*e1)->heure  = e2->heure;
    (*e1)->minute = e2->minute;
}

/* ---- elementComparer : compare par id ---- */
int elementComparer(ELEMENT e1, ELEMENT e2) {
    if (!e1 || !e2) return 0;
    return e1->id - e2->id;
}

/* ---- elementLire : saisie au clavier avec validation ---- */
void elementLire(ELEMENT *e) {
    int valid;

    if (!e || !(*e)) return;

    /* ID validation */
    do {
        valid = 1;
        printf("  ID            : ");
        if (scanf("%d", &(*e)->id) != 1) {
            printf("  Erreur : ID doit etre un nombre entier\n");
            viderBuffer();
            valid = 0;
        } else {
            viderBuffer();
        }
    } while (!valid);

    /* Titre */
    printf("  Titre         : ");
    scanf("%99s", (*e)->titre);
    viderBuffer();

    /* Source */
    printf("  Source        : ");
    scanf("%49s", (*e)->source);
    viderBuffer();

    /* Score validation (0-100) */
    do {
        valid = 1;
        printf("  Score (0-100) : ");
        if (scanf("%d", &(*e)->score_fiabilite) != 1 ||
            (*e)->score_fiabilite < 0 || (*e)->score_fiabilite > 100) {
            printf("  Erreur : Score doit etre entre 0 et 100\n");
            viderBuffer();
            valid = 0;
        } else {
            viderBuffer();
        }
    } while (!valid);

    /* Date validation */
    do {
        valid = 1;
        printf("  Date (j m a)  : ");
        if (scanf("%d %d %d", &(*e)->jour, &(*e)->mois, &(*e)->annee) != 3) {
            printf("  Erreur : Entrez 3 nombres (jour mois annee)\n");
            viderBuffer();
            valid = 0;
        } else if ((*e)->jour < 1 || (*e)->jour > 31 ||
                   (*e)->mois < 1 || (*e)->mois > 12) {
            printf("  Erreur : Jour (1-31) et Mois (1-12) invalides\n");
            valid = 0;
        } else {
            viderBuffer();
        }
    } while (!valid);

    /* Time validation */
    do {
        valid = 1;
        printf("  Heure (h min) : ");
        if (scanf("%d %d", &(*e)->heure, &(*e)->minute) != 2 ||
            (*e)->heure < 0 || (*e)->heure > 23 ||
            (*e)->minute < 0 || (*e)->minute > 59) {
            printf("  Erreur : Heure (0-23) et Minute (0-59) invalides\n");
            viderBuffer();
            valid = 0;
        } else {
            viderBuffer();
        }
    } while (!valid);
}

/* ---- elementAfficher : affichage d'un article ---- */
void elementAfficher(ELEMENT e) {
    if (!e) { printf("(element vide)"); return; }
    printf("%s (id:%d, source:%s, score:%d, %02d/%02d/%d %02dh%02d)",
           e->titre, e->id, e->source, e->score_fiabilite,
           e->jour, e->mois, e->annee, e->heure, e->minute);
}
