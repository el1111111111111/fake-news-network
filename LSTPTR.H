
#ifndef _LSTPTR_H
#define _LSTPTR_H

#include "ELTPRIM.H"

typedef struct structNoeud {
    ELEMENT info;
    struct structNoeud *suivant;
} structNoeud, *NOEUD;

typedef struct {
    NOEUD tete;
    int   lg;
} laStruct, *LISTE;

#endif
