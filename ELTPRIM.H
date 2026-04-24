
#ifndef _ELTPRIM_H
#define _ELTPRIM_H

#include "ELTSDD.H"

/* Lecture d'un element */
void elementLire(ELEMENT *e);

/* Affichage d'un element */
void elementAfficher(ELEMENT e);

/* Affectation : e1 = e2 (pointent sur le meme objet) */
void elementAffecter(ELEMENT *e1, ELEMENT e2);

/* Copie profonde du contenu de e2 dans e1 */
void elementCopier(ELEMENT *e1, ELEMENT e2);

/* Comparaison : retourne e1->id - e2->id */
int  elementComparer(ELEMENT e1, ELEMENT e2);

/* Creation (allocation) d'un element vide */
ELEMENT elementCreer(void);

/* Liberation memoire d'un element */
void elementDetruire(ELEMENT e);

#endif
