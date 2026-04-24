
#ifndef _LSTPRIM_H
#define _LSTPRIM_H

#include "ELTPRIM.H"
#include "LSTSDD.H"

LISTE listeCreer(void);
void listeDetruire(LISTE L);
int estVide(LISTE L);
int estSaturee(LISTE L);
int listeTaille(LISTE L);
ELEMENT recuperer(LISTE L, int pos);
int inserer(LISTE L, ELEMENT e, int pos);
int supprimer(LISTE L, int pos);
void listeAfficher(LISTE L);
LISTE listeCopier(LISTE L);
int listeComparer(LISTE L1, LISTE L2);

#endif
