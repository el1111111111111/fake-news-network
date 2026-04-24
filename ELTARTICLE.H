
#ifndef _ELTARTICLE_H
#define _ELTARTICLE_H

typedef struct {
    int  id;
    char titre[100];
    char source[50];
    int  score_fiabilite;
    int  jour, mois, annee;
    int  heure, minute;
} articleStruct, *ELEMENT;

#define ELEMENT_VIDE NULL

#endif
