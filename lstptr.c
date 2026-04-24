#include <stdio.h>
#include <stdlib.h>
#include "LSTPRIM.H"

/* ============================================================
   Fonctions internes sur les noeuds (non exposees dans .H)
   ============================================================ */

static NOEUD noeudCreer(ELEMENT e) {
    NOEUD n = (NOEUD)malloc(sizeof(structNoeud));
    if (!n)
        printf("\nPlus d'espace");
    else {
        elementAffecter(&n->info, e);   /* stockage indirect : copie du pointeur */
        n->suivant = NULL;
    }
    return n;
}

static void noeudDetruire(NOEUD n) {

    free(n);
}

/* ============================================================
   Primitives du TDA LISTE
   ============================================================ */

LISTE listeCreer(void) {
    LISTE L = (LISTE)malloc(sizeof(laStruct));
    if (!L)
        printf("\nProbleme de memoire");
    else {
        L->lg   = 0;
        L->tete = NULL;
    }
    return L;
}

void listeDetruire(LISTE L) {
    int i;
    NOEUD p, q;
    if (!L) return;
    q = L->tete;
    for (i = 1; i <= L->lg; i++) {
        p = q;
        q = q->suivant;
        noeudDetruire(p);
    }
    free(L);
}

int estVide(LISTE L) {
    return (L->lg == 0);
}

int estSaturee(LISTE L) {
    NOEUD temp;
    int saturee = 1;
    (void)L;
    temp = (NOEUD)malloc(sizeof(structNoeud));
    if (temp != NULL) {
        saturee = 0;
        free(temp);
    }
    return saturee;
}

int listeTaille(LISTE L) {
    if (!L) return 0;
    return L->lg;
}

/* ---- inserer : ajoute e a la position pos (1-based) ---- */
int inserer(LISTE L, ELEMENT e, int pos) {
    int succee = 1;
    int i;
    NOEUD n, p, q;

    if (!L) return 0;

    if (estSaturee(L)) {
        printf("\nListe saturee");
        succee = 0;
    } else {
        if (pos < 1 || pos > L->lg + 1) {
            printf("\nPosition invalide");
            succee = 0;
        } else {
            n = noeudCreer(e);
            if (pos == 1) {          /* insertion en tete */
                n->suivant = L->tete;
                L->tete    = n;
            } else {                 /* cas general pos > 1 */
                q = L->tete;
                for (i = 1; i < pos; i++) {
                    p = q;
                    q = q->suivant;
                }
                /* q = noeud de rang pos, p = predecesseur */
                p->suivant = n;
                n->suivant = q;
            }
            (L->lg)++;
        }
    }
    return succee;
}

/* ---- supprimer : supprime l'element a la position pos (1-based) ---- */
int supprimer(LISTE L, int pos) {
    int i;
    int succee = 1;
    NOEUD p = NULL, q;

    if (!L) return 0;

    if (estVide(L)) {
        printf("\nListe vide");
        succee = 0;
    } else {
        if (pos < 1 || pos > L->lg) {
            printf("\nPosition invalide");
            succee = 0;
        } else {
            q = L->tete;
            if (pos == 1) {          /* suppression en tete */
                L->tete = L->tete->suivant;
            } else {                 /* cas general pos > 1 */
                for (i = 1; i < pos; i++) {
                    p = q;
                    q = q->suivant;
                }
                /* q = noeud a supprimer, p = predecesseur */
                p->suivant = q->suivant;
            }
            noeudDetruire(q);
            (L->lg)--;
        }
    }
    return succee;
}

/* ---- recuperer : retourne l'element a la position pos (1-based) ---- */
ELEMENT recuperer(LISTE L, int pos) {
    ELEMENT elt = ELEMENT_VIDE;
    int i;
    NOEUD p;

    if (!L) return ELEMENT_VIDE;

    if (estVide(L))
        printf("\nListe vide");
    else {
        if (pos < 1 || pos > L->lg)
            printf("\nPosition invalide");
        else {
            p = L->tete;
            for (i = 1; i < pos; i++)
                p = p->suivant;
            /* Stockage indirect : on retourne directement le pointeur
               (modifier les champs via le resultat modifie la liste) */
            elementAffecter(&elt, p->info);
        }
    }
    return elt;
}

/* ---- listeAfficher ---- */
void listeAfficher(LISTE L) {
    int i;
    NOEUD p;
    if (!L) return;
    p = L->tete;
    for (i = 1; i <= L->lg; i++) {
        elementAfficher(p->info);
        printf("\n");
        p = p->suivant;
    }
}

/* ---- listeCopier : copie profonde ---- */
LISTE listeCopier(LISTE L) {
    LISTE LR = listeCreer();
    int i;
    ELEMENT elt;
    for (i = 1; i <= L->lg; i++) {
        elt = elementCreer();
        elementCopier(&elt, recuperer(L, i));
        inserer(LR, elt, i);
    }
    return LR;
}

/* ---- listeComparer ---- */
int listeComparer(LISTE L1, LISTE L2) {
    int test = 1;
    int i    = 1;
    if (listeTaille(L1) != listeTaille(L2)) test = 0;
    while (i <= listeTaille(L1) && test) {
        if (elementComparer(recuperer(L1, i), recuperer(L2, i)) != 0)
            test = 0;
        i++;
    }
    return test;
}
