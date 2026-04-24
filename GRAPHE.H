
#ifndef _GRAPHE_H
#define _GRAPHE_H

#include "LSTPRIM.H"

typedef struct {
    int      V;           /* nombre d'articles */
    ELEMENT *articles;    /* tableau articles[0..V-1]       */
    LISTE   *adjList;     /* adjList[i] = liste des articles cites par i */
    int     *degre_in;    /* degre_in[i] = nb d'articles qui citent i   */
} grapheRStructure, *grapheReseau;

/* ------ Construction / Destruction ------ */
grapheReseau creerGraphe(int V);
grapheReseau chargerGraphe(const char *filename);
void         detruireGraphe(grapheReseau g);

/* ------ Modification ------ */
int ajouterArticle(grapheReseau g, ELEMENT art);
int supprimerArticle(grapheReseau g, int idArt);
int ajouterCitation(grapheReseau g, int idSrc, int idDest);
int supprimerCitation(grapheReseau g, int idSrc, int idDest);

/* ------ Affichage ------ */
void afficherGraphe(grapheReseau g);

/* ------ Section 2 : Interrogation ------ */
void    articlesCites(grapheReseau g, int idSrc);
void    articlesCitants(grapheReseau g, int idDest);
void    sourcesOriginales(grapheReseau g);
void    articlesIsoles(grapheReseau g);
ELEMENT articlePlusCite(grapheReseau g);

/* ------ Section 3 : Analyse chronologique ------ */
int  comparerDates(ELEMENT art1, ELEMENT art2);
void trierParDate(grapheReseau g);
void premierCitant(grapheReseau g, int idDest);
void chainePropagation(grapheReseau g, int idSrc);

/* ------ Section 4 : BFS ------ */
void simulerPropagation(grapheReseau g, int idSrc);
void articlesAccessibles(grapheReseau g, int idSrc);

/* ------ Section 5 : Detection fake news ------ */
int  analyserArticle(ELEMENT art);
void analyserReseau(grapheReseau g);
void articlesSuspectsCites(grapheReseau g);

/* ------ Section 6 : Bonus ------ */
void simulerSuppression(grapheReseau g, int idArt);
int  neutraliserPropagation(grapheReseau g, int idSrc, int idDest);

#endif
