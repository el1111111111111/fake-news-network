
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "GRAPHE.H"
#include "fakesdb.h"

/* ============================================================
   HELPERS INTERNES
   ============================================================ */

/* Convertit src en minuscules dans dst (taille maxLen) */
static void strToLower(const char *src, char *dst, int maxLen) {
    int i;
    for (i = 0; src[i] && i < maxLen - 1; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

/* Remplace les '_' par des espaces dans s */
static void replaceUnderscore(char *s) {
    for (; *s; s++)
        if (*s == '_') *s = ' ';
}

/* ============================================================
   SECTION 1A : CONSTRUCTION / DESTRUCTION
   ============================================================ */

grapheReseau creerGraphe(int V) {
    int i;
    grapheReseau g = (grapheReseau)malloc(sizeof(grapheRStructure));
    if (!g) { printf("\nMemoire insuffisante"); return NULL; }

    g->V        = V;
    g->articles = (ELEMENT*)calloc(V, sizeof(ELEMENT));
    g->adjList  = (LISTE*)  malloc(V * sizeof(LISTE));
    g->degre_in = (int*)    calloc(V, sizeof(int));

    if (!g->articles || !g->adjList || !g->degre_in) {
        free(g->articles); free(g->adjList); free(g->degre_in); free(g);
        return NULL;
    }
    for (i = 0; i < V; i++) {
        g->articles[i] = ELEMENT_VIDE;
        g->adjList[i]  = listeCreer();
        g->degre_in[i] = 0;
    }
    return g;
}

grapheReseau chargerGraphe(const char *filename) {
    FILE *f;
    char line[512];
    int  maxId = -1, i;
    grapheReseau g;

    f = fopen(filename, "r");
    if (!f) { printf("Erreur : impossible d'ouvrir '%s'\n", filename); return NULL; }

    /* 1re passe : trouver le nombre d'articles */
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        if (line[0] == 'A') {
            int id;
            if (sscanf(line + 2, "%d", &id) == 1 && id > maxId)
                maxId = id;
        }
    }
    if (maxId < 0) { fclose(f); return NULL; }

    g = creerGraphe(maxId + 1);
    if (!g) { fclose(f); return NULL; }

    /* 2e passe : charger les articles puis les citations */
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        char *comment;

        /* Supprimer commentaire inline */
        comment = strchr(line, '#');
        if (line[0] == '#') continue;
        if (comment) *comment = '\0';

        if (line[0] == 'A') {
            /* Format : A id "titre" source score jour mois annee heure minute */
            int id, score, jour, mois, annee, heure, minute;
            char titre[100], source[50];
            char *p = line + 2;
            ELEMENT art;

            while (*p == ' ') p++;
            if (sscanf(p, "%d", &id) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;

            /* Titre : entre guillemets ou mot simple */
            if (*p == '"') {
                int ti = 0;
                p++;
                while (*p && *p != '"' && ti < 99) titre[ti++] = *p++;
                titre[ti] = '\0';
                if (*p == '"') p++;
            } else {
                int ti = 0;
                while (*p && *p != ' ' && ti < 99) titre[ti++] = *p++;
                titre[ti] = '\0';
            }
            while (*p == ' ') p++;

            /* Source + champs numeriques */
            if (sscanf(p, "%49s %d %d %d %d %d %d %d",
                       source, &score, &jour, &mois, &annee, &heure, &minute,
                       &heure) < 7) continue;
            /* Relecture propre des 7 champs */
            sscanf(p, "%49s %d %d %d %d %d %d",
                   source, &score, &jour, &mois, &annee, &heure, &minute);

            art = elementCreer();
            if (!art) continue;
            art->id             = id;
            strncpy(art->titre,  titre,  99);  art->titre[99]  = '\0';
            strncpy(art->source, source, 49);  art->source[49] = '\0';
            art->score_fiabilite = score;
            art->jour = jour; art->mois = mois; art->annee = annee;
            art->heure = heure; art->minute = minute;

            if (id < g->V) g->articles[id] = art;

        } else if (line[0] == 'C') {
            int src, dest;
            if (sscanf(line + 2, "%d %d", &src, &dest) == 2)
                ajouterCitation(g, src, dest);
        }
    }
    fclose(f);
    (void)i;
    return g;
}

void detruireGraphe(grapheReseau g) {
    int i;
    if (!g) return;
    for (i = 0; i < g->V; i++) {
        if (g->articles[i]) elementDetruire(g->articles[i]);
        listeDetruire(g->adjList[i]);
    }
    free(g->articles);
    free(g->adjList);
    free(g->degre_in);
    free(g);
}

/* ============================================================
   SECTION 1B : MODIFICATION
   ============================================================ */

int ajouterArticle(grapheReseau g, ELEMENT art) {
    int i, newV;
    if (!g || !art) return 0;
    if (art->id < 0) {
        printf("Erreur : ID negatif.\n");
        return 0;
    }

    /* Si l'ID depasse la capacite actuelle, agrandir le graphe */
    if (art->id >= g->V) {
        newV = art->id + 1;
        g->articles = (ELEMENT*)realloc(g->articles, newV * sizeof(ELEMENT));
        g->adjList  = (LISTE*)  realloc(g->adjList,  newV * sizeof(LISTE));
        g->degre_in = (int*)    realloc(g->degre_in, newV * sizeof(int));
        if (!g->articles || !g->adjList || !g->degre_in) {
            printf("Erreur : memoire insuffisante pour agrandir le graphe.\n");
            return 0;
        }
        for (i = g->V; i < newV; i++) {
            g->articles[i] = ELEMENT_VIDE;
            g->adjList[i]  = listeCreer();
            g->degre_in[i] = 0;
        }
        g->V = newV;
    }

    if (g->articles[art->id] != ELEMENT_VIDE) {
        printf("Erreur : un article avec l'ID %d existe deja.\n", art->id);
        return 0;
    }
    g->articles[art->id] = art;
    return 1;
}

int supprimerArticle(grapheReseau g, int idArt) {
    int i, j;
    if (!g || idArt < 0 || idArt >= g->V || !g->articles[idArt]) return 0;

    /* Supprimer les citations sortantes de idArt */
    while (listeTaille(g->adjList[idArt]) > 0) {
        ELEMENT dest = recuperer(g->adjList[idArt], 1);
        if (dest) g->degre_in[dest->id]--;
        supprimer(g->adjList[idArt], 1);
    }

    /* Supprimer idArt des listes d'adjacence des autres */
    for (i = 0; i < g->V; i++) {
        if (i == idArt || !g->articles[i]) continue;
        j = 1;
        while (j <= listeTaille(g->adjList[i])) {
            ELEMENT e = recuperer(g->adjList[i], j);
            if (e && e->id == idArt) {
                supprimer(g->adjList[i], j);
                g->degre_in[idArt]--;
            } else {
                j++;
            }
        }
    }

    elementDetruire(g->articles[idArt]);
    g->articles[idArt] = ELEMENT_VIDE;
    return 1;
}

int ajouterCitation(grapheReseau g, int idSrc, int idDest) {
    int j;
    if (!g || idSrc < 0 || idSrc >= g->V || idDest < 0 || idDest >= g->V) return 0;
    if (!g->articles[idSrc] || !g->articles[idDest]) return 0;

    /* Verifier unicite */
    for (j = 1; j <= listeTaille(g->adjList[idSrc]); j++) {
        ELEMENT e = recuperer(g->adjList[idSrc], j);
        if (e && e->id == idDest) return 0;
    }

    inserer(g->adjList[idSrc], g->articles[idDest],
            listeTaille(g->adjList[idSrc]) + 1);
    g->degre_in[idDest]++;
    return 1;
}

int supprimerCitation(grapheReseau g, int idSrc, int idDest) {
    int j;
    if (!g || idSrc < 0 || idSrc >= g->V || idDest < 0 || idDest >= g->V) return 0;
    for (j = 1; j <= listeTaille(g->adjList[idSrc]); j++) {
        ELEMENT e = recuperer(g->adjList[idSrc], j);
        if (e && e->id == idDest) {
            supprimer(g->adjList[idSrc], j);
            g->degre_in[idDest]--;
            return 1;
        }
    }
    return 0;
}

/* ============================================================
   SECTION 1C : AFFICHAGE
   ============================================================ */

void afficherGraphe(grapheReseau g) {
    int i, j;
    if (!g) { printf("Graphe vide.\n"); return; }
    for (i = 0; i < g->V; i++) {
        ELEMENT art = g->articles[i];
        if (!art) continue;
        printf("%s (id:%d, source:%s, score:%d, %02d/%02d/%d %02dh%02d)\n",
               art->titre, art->id, art->source, art->score_fiabilite,
               art->jour, art->mois, art->annee, art->heure, art->minute);
        if (listeTaille(g->adjList[i]) == 0) {
            printf("   (ne cite aucun article)\n");
        } else {
            for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                ELEMENT dest = recuperer(g->adjList[i], j);
                if (dest) printf("   --> %s\n", dest->titre);
            }
        }
    }
}

/* ============================================================
   SECTION 2 : INTERROGATION
   ============================================================ */

void articlesCites(grapheReseau g, int idSrc) {
    int j;
    if (!g || idSrc < 0 || idSrc >= g->V || !g->articles[idSrc]) {
        printf("Article introuvable.\n"); return;
    }
    if (listeTaille(g->adjList[idSrc]) == 0) {
        printf("Cet article ne cite aucun autre article.\n"); return;
    }
    for (j = 1; j <= listeTaille(g->adjList[idSrc]); j++) {
        ELEMENT e = recuperer(g->adjList[idSrc], j);
        if (e) printf("--> %s\n", e->titre);
    }
}

void articlesCitants(grapheReseau g, int idDest) {
    int i, j, trouve = 0;
    if (!g || idDest < 0 || idDest >= g->V || !g->articles[idDest]) {
        printf("Article introuvable.\n"); return;
    }
    for (i = 0; i < g->V; i++) {
        if (!g->articles[i]) continue;
        for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
            ELEMENT e = recuperer(g->adjList[i], j);
            if (e && e->id == idDest) {
                printf("--> %s\n", g->articles[i]->titre);
                trouve = 1;
            }
        }
    }
    if (!trouve) printf("Aucun article ne cite cet article.\n");
}

void sourcesOriginales(grapheReseau g) {
    int i, trouve = 0;
    if (!g) return;
    for (i = 0; i < g->V; i++) {
        if (!g->articles[i]) continue;
        if (listeTaille(g->adjList[i]) == 0) {
            printf("--> %s (ne cite personne)\n", g->articles[i]->titre);
            trouve = 1;
        }
    }
    if (!trouve) printf("Aucune source originale.\n");
}

void articlesIsoles(grapheReseau g) {
    int i, trouve = 0;
    if (!g) return;
    for (i = 0; i < g->V; i++) {
        if (!g->articles[i]) continue;
        if (g->degre_in[i] == 0) {
            printf("--> %s (non cite par personne)\n", g->articles[i]->titre);
            trouve = 1;
        }
    }
    if (!trouve) printf("Aucun article isole.\n");
}

ELEMENT articlePlusCite(grapheReseau g) {
    int i, maxDeg = -1;
    ELEMENT premier = ELEMENT_VIDE;
    if (!g) return ELEMENT_VIDE;

    for (i = 0; i < g->V; i++)
        if (g->articles[i] && g->degre_in[i] > maxDeg)
            maxDeg = g->degre_in[i];

    if (maxDeg < 0) return ELEMENT_VIDE;

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] && g->degre_in[i] == maxDeg) {
            printf("--> %s (cite par %d articles)\n",
                   g->articles[i]->titre, g->degre_in[i]);
            if (!premier) premier = g->articles[i];
        }
    }
    return premier;
}

/* ============================================================
   SECTION 3 : ANALYSE CHRONOLOGIQUE
   ============================================================ */

int comparerDates(ELEMENT art1, ELEMENT art2) {
    if (!art1 || !art2) return 0;
    if (art1->annee  != art2->annee)  return art1->annee  - art2->annee;
    if (art1->mois   != art2->mois)   return art1->mois   - art2->mois;
    if (art1->jour   != art2->jour)   return art1->jour   - art2->jour;
    if (art1->heure  != art2->heure)  return art1->heure  - art2->heure;
    return art1->minute - art2->minute;
}

void trierParDate(grapheReseau g) {
    /* Tri par insertion sur un tableau local des articles presents */
    ELEMENT tab[512];
    int n = 0, i, j;
    ELEMENT cle;

    if (!g) return;
    for (i = 0; i < g->V; i++)
        if (g->articles[i]) tab[n++] = g->articles[i];

    /* Tri par insertion (demande par le sujet) */
    for (i = 1; i < n; i++) {
        cle = tab[i];
        j   = i - 1;
        while (j >= 0 && comparerDates(tab[j], cle) > 0) {
            tab[j + 1] = tab[j];
            j--;
        }
        tab[j + 1] = cle;
    }

    for (i = 0; i < n; i++)
        printf("%d. %s (%02d/%02d/%d %02dh%02d)\n",
               i + 1, tab[i]->titre,
               tab[i]->jour, tab[i]->mois, tab[i]->annee,
               tab[i]->heure, tab[i]->minute);
}

void premierCitant(grapheReseau g, int idDest) {
    int i, j;
    ELEMENT premier = ELEMENT_VIDE;

    if (!g || idDest < 0 || idDest >= g->V || !g->articles[idDest]) {
        printf("Article introuvable.\n"); return;
    }
    for (i = 0; i < g->V; i++) {
        if (!g->articles[i]) continue;
        for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
            ELEMENT e = recuperer(g->adjList[i], j);
            if (e && e->id == idDest) {
                if (!premier || comparerDates(g->articles[i], premier) < 0)
                    premier = g->articles[i];
            }
        }
    }
    if (premier)
        printf("--> %s (%02d/%02d/%d %02dh%02d)\n",
               premier->titre,
               premier->jour, premier->mois, premier->annee,
               premier->heure, premier->minute);
    else
        printf("Aucun article ne cite cet article.\n");
}

void chainePropagation(grapheReseau g, int idSrc) {
    /*
     * Reconstitue la chaine chronologique depuis idSrc :
     * on fait un BFS "vers l'avant" (qui cite idSrc, puis qui cite ceux-la...)
     * puis on trie par date et on affiche.
     */
    int *visite;
    LISTE file, chaine;
    ELEMENT tab[512];
    int n = 0, i, j;
    ELEMENT cle;

    if (!g || idSrc < 0 || idSrc >= g->V || !g->articles[idSrc]) {
        printf("Article introuvable.\n"); return;
    }

    visite = (int*)calloc(g->V, sizeof(int));
    file   = listeCreer();
    chaine = listeCreer();

    visite[idSrc] = 1;
    inserer(file,   g->articles[idSrc], listeTaille(file)   + 1);
    inserer(chaine, g->articles[idSrc], listeTaille(chaine) + 1);

    while (!estVide(file)) {
        ELEMENT courant = recuperer(file, 1);
        supprimer(file, 1);

        for (i = 0; i < g->V; i++) {
            if (!g->articles[i] || visite[i]) continue;
            for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                ELEMENT e = recuperer(g->adjList[i], j);
                if (e && e->id == courant->id) {
                    visite[i] = 1;
                    inserer(file,   g->articles[i], listeTaille(file)   + 1);
                    inserer(chaine, g->articles[i], listeTaille(chaine) + 1);
                }
            }
        }
    }
    listeDetruire(file);
    free(visite);

    /* Copier la chaine dans un tableau et trier par date */
    n = listeTaille(chaine);
    for (i = 0; i < n; i++) tab[i] = recuperer(chaine, i + 1);
    listeDetruire(chaine);

    for (i = 1; i < n; i++) {        /* tri par insertion */
        cle = tab[i]; j = i - 1;
        while (j >= 0 && comparerDates(tab[j], cle) > 0) { tab[j+1]=tab[j]; j--; }
        tab[j+1] = cle;
    }

    /* Affichage */
    printf("%s (%02d/%02d/%d %02dh%02d)\n",
           tab[0]->titre, tab[0]->jour, tab[0]->mois, tab[0]->annee,
           tab[0]->heure, tab[0]->minute);
    for (i = 1; i < n; i++)
        printf("   --> cite par %s (%02d/%02d/%d %02dh%02d)\n",
               tab[i]->titre, tab[i]->jour, tab[i]->mois, tab[i]->annee,
               tab[i]->heure, tab[i]->minute);
}

/* ============================================================
   SECTION 4 : BFS (file realisee avec le TDA LISTE)
   Enfiler  : inserer(file, e, listeTaille(file)+1)
   Defiler  : recuperer(file,1) puis supprimer(file,1)
   ============================================================ */

void simulerPropagation(grapheReseau g, int idSrc) {
    int *visite, *niveau;
    LISTE file;
    int i, j, niveauMax = 0, totalAtteints = 0;

    if (!g || idSrc < 0 || idSrc >= g->V || !g->articles[idSrc]) {
        printf("Article introuvable.\n"); return;
    }

    visite = (int*)calloc(g->V, sizeof(int));
    niveau = (int*)calloc(g->V, sizeof(int));
    file   = listeCreer();

    visite[idSrc] = 1;
    niveau[idSrc] = 0;
    /* Enfiler */
    inserer(file, g->articles[idSrc], listeTaille(file) + 1);

    while (!estVide(file)) {
        /* Defiler */
        ELEMENT courant;
        int niv;
        courant = recuperer(file, 1);
        supprimer(file, 1);

        niv = niveau[courant->id];
        if (niv > niveauMax) niveauMax = niv;
        totalAtteints++;

        for (i = 0; i < g->V; i++) {
            if (!g->articles[i] || visite[i]) continue;
            for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                ELEMENT e = recuperer(g->adjList[i], j);
                if (e && e->id == courant->id && !visite[i]) {
                    visite[i] = 1;
                    niveau[i] = niv + 1;
                    /* Enfiler */
                    inserer(file, g->articles[i], listeTaille(file) + 1);
                }
            }
        }
    }
    listeDetruire(file);

    /* Affichage par niveaux */
    for (i = 0; i <= niveauMax; i++) {
        int first = 1;
        printf("Niveau %d : ", i);
        for (j = 0; j < g->V; j++) {
            if (visite[j] && niveau[j] == i && g->articles[j]) {
                if (!first) printf(", ");
                printf("%s", g->articles[j]->titre);
                first = 0;
            }
        }
        printf("\n");
    }
    printf("%d niveaux, %d articles atteints.\n", niveauMax + 1, totalAtteints);

    free(visite);
    free(niveau);
}

void articlesAccessibles(grapheReseau g, int idSrc) {
    int *visite, i, j;
    LISTE file;

    if (!g || idSrc < 0 || idSrc >= g->V || !g->articles[idSrc]) {
        printf("Article introuvable.\n"); return;
    }

    visite = (int*)calloc(g->V, sizeof(int));
    file   = listeCreer();

    visite[idSrc] = 1;
    inserer(file, g->articles[idSrc], 1);
    printf("%s\n", g->articles[idSrc]->titre);

    while (!estVide(file)) {
        ELEMENT courant = recuperer(file, 1);
        supprimer(file, 1);

        for (i = 0; i < g->V; i++) {
            if (!g->articles[i] || visite[i]) continue;
            for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                ELEMENT e = recuperer(g->adjList[i], j);
                if (e && e->id == courant->id && !visite[i]) {
                    visite[i] = 1;
                    printf("%s\n", g->articles[i]->titre);
                    inserer(file, g->articles[i], listeTaille(file) + 1);
                }
            }
        }
    }
    listeDetruire(file);
    free(visite);
}

/* ============================================================
   SECTION 5 : DETECTION FAKE NEWS
   ============================================================ */

int analyserArticle(ELEMENT art) {
    int score_suspicion = 0, k;
    char titre_lower[100];

    if (!art) return 0;

    strToLower(art->titre, titre_lower, sizeof(titre_lower));
    replaceUnderscore(titre_lower);

    /* +40 si contient un fragment de BASE_FAKES */
    for (k = 0; k < NB_FAKES; k++) {
        if (strstr(titre_lower, BASE_FAKES[k])) {
            score_suspicion += 40;
            break;
        }
    }

    /* +10 par mot suspect */
    for (k = 0; k < NB_SUSPECTS; k++)
        if (strstr(titre_lower, MOTS_SUSPECTS[k]))
            score_suspicion += 10;

    art->score_fiabilite = (100 - score_suspicion < 0) ? 0 : 100 - score_suspicion;
    return score_suspicion;
}

void analyserReseau(grapheReseau g) {
    int i, k;
    if (!g) return;
    for (i = 0; i < g->V; i++) {
        ELEMENT art = g->articles[i];
        char titre_lower[100];
        char motsTrouves[256];
        int  first;

        if (!art) continue;
        analyserArticle(art);

        strToLower(art->titre, titre_lower, sizeof(titre_lower));
        replaceUnderscore(titre_lower);

        if (art->score_fiabilite < 40) {
            printf("[SUSPECT] %s (score: %d)\n", art->titre, art->score_fiabilite);
            for (k = 0; k < NB_FAKES; k++)
                if (strstr(titre_lower, BASE_FAKES[k]))
                    printf("   \"%s\"\n", BASE_FAKES[k]);
            motsTrouves[0] = '\0'; first = 1;
            for (k = 0; k < NB_SUSPECTS; k++) {
                if (strstr(titre_lower, MOTS_SUSPECTS[k])) {
                    if (!first) strcat(motsTrouves, ", ");
                    strcat(motsTrouves, MOTS_SUSPECTS[k]);
                    first = 0;
                }
            }
            if (motsTrouves[0]) printf("   mots: %s\n", motsTrouves);

        } else if (art->score_fiabilite < 70) {
            printf("[DOUTEUX] %s (score: %d)\n", art->titre, art->score_fiabilite);
        } else {
            printf("[FIABLE ] %s (score: %d)\n", art->titre, art->score_fiabilite);
        }
    }
}

void articlesSuspectsCites(grapheReseau g) {
    ELEMENT suspects[512];
    int n = 0, i;
    ELEMENT tmp;

    if (!g) return;
    for (i = 0; i < g->V; i++) {
        ELEMENT art = g->articles[i];
        if (art && art->score_fiabilite < 40 && g->degre_in[i] > 0)
            suspects[n++] = art;
    }

    /* Tri par selection : degre_in decroissant */
    for (i = 0; i < n - 1; i++) {
        int maxIdx = i, j;
        for (j = i + 1; j < n; j++)
            if (g->degre_in[suspects[j]->id] > g->degre_in[suspects[maxIdx]->id])
                maxIdx = j;
        tmp = suspects[i]; suspects[i] = suspects[maxIdx]; suspects[maxIdx] = tmp;
    }

    if (n == 0) { printf("Aucun article suspect cite.\n"); return; }
    for (i = 0; i < n; i++)
        printf("%s (score:%d, cite par %d articles)\n",
               suspects[i]->titre, suspects[i]->score_fiabilite,
               g->degre_in[suspects[i]->id]);
}

/* ============================================================
   SECTION 6 : BONUS
   ============================================================ */

/* BFS depuis src, retourne 1 si dest est accessible */
static int cheminExiste(grapheReseau g, int src, int dest) {
    int *visite, i, j, trouve = 0;
    LISTE file;

    if (!g->articles[src] || !g->articles[dest]) return 0;

    visite = (int*)calloc(g->V, sizeof(int));
    file   = listeCreer();

    visite[src] = 1;
    inserer(file, g->articles[src], 1);

    while (!estVide(file) && !trouve) {
        ELEMENT courant = recuperer(file, 1);
        supprimer(file, 1);
        if (courant->id == dest) { trouve = 1; break; }

        for (i = 0; i < g->V; i++) {
            if (!g->articles[i] || visite[i]) continue;
            for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                ELEMENT e = recuperer(g->adjList[i], j);
                if (e && e->id == courant->id && !visite[i]) {
                    visite[i] = 1;
                    if (i == dest) { trouve = 1; break; }
                    inserer(file, g->articles[i], listeTaille(file) + 1);
                }
            }
            if (trouve) break;
        }
    }
    listeDetruire(file);
    free(visite);
    return trouve;
}

void simulerSuppression(grapheReseau g, int idArt) {
    int i, j, nb_citants = 0, nb_cites, *avant, *apres, nb_deconn = 0;
    int src = -1;
    LISTE file;

    if (!g || idArt < 0 || idArt >= g->V || !g->articles[idArt]) {
        printf("Article introuvable.\n"); return;
    }

    printf("Suppression de %s :\n", g->articles[idArt]->titre);

    /* Articles qui le citaient */
    printf("- Etait cite par : ");
    for (i = 0; i < g->V; i++) {
        if (!g->articles[i]) continue;
        for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
            ELEMENT e = recuperer(g->adjList[i], j);
            if (e && e->id == idArt) {
                if (nb_citants > 0) printf(", ");
                printf("%s", g->articles[i]->titre);
                nb_citants++;
            }
        }
    }
    printf(" (%d)\n", nb_citants);

    /* Articles qu'il citait */
    nb_cites = listeTaille(g->adjList[idArt]);
    printf("- Citait : ");
    for (j = 1; j <= nb_cites; j++) {
        ELEMENT e = recuperer(g->adjList[idArt], j);
        if (e) { if (j > 1) printf(", "); printf("%s", e->titre); }
    }
    printf(" (%d)\n", nb_cites);

    /* Accessibilite avant suppression (BFS depuis la source) */
    avant = (int*)calloc(g->V, sizeof(int));
    for (i = 0; i < g->V; i++)
        if (g->articles[i] && g->degre_in[i] == 0) { src = i; break; }

    if (src >= 0) {
        int *vis = (int*)calloc(g->V, sizeof(int));
        file = listeCreer();
        vis[src] = 1; avant[src] = 1;
        inserer(file, g->articles[src], 1);
        while (!estVide(file)) {
            ELEMENT c = recuperer(file, 1); supprimer(file, 1);
            for (i = 0; i < g->V; i++) {
                if (!g->articles[i] || vis[i]) continue;
                for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                    ELEMENT e = recuperer(g->adjList[i], j);
                    if (e && e->id == c->id && !vis[i]) {
                        vis[i] = 1; avant[i] = 1;
                        inserer(file, g->articles[i], listeTaille(file)+1);
                    }
                }
            }
        }
        listeDetruire(file); free(vis);
    }

    /* Supprimer l'article */
    supprimerArticle(g, idArt);

    /* Accessibilite apres suppression */
    apres = (int*)calloc(g->V, sizeof(int));
    src = -1;
    for (i = 0; i < g->V; i++)
        if (g->articles[i] && g->degre_in[i] == 0) { src = i; break; }

    if (src >= 0) {
        int *vis = (int*)calloc(g->V, sizeof(int));
        file = listeCreer();
        vis[src] = 1; apres[src] = 1;
        inserer(file, g->articles[src], 1);
        while (!estVide(file)) {
            ELEMENT c = recuperer(file, 1); supprimer(file, 1);
            for (i = 0; i < g->V; i++) {
                if (!g->articles[i] || vis[i]) continue;
                for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                    ELEMENT e = recuperer(g->adjList[i], j);
                    if (e && e->id == c->id && !vis[i]) {
                        vis[i] = 1; apres[i] = 1;
                        inserer(file, g->articles[i], listeTaille(file)+1);
                    }
                }
            }
        }
        listeDetruire(file); free(vis);
    }

    printf("- Articles deconnectes : ");
    for (i = 0; i < g->V; i++) {
        if (avant[i] && !apres[i] && g->articles[i]) {
            if (nb_deconn > 0) printf(", ");
            printf("%s", g->articles[i]->titre);
            nb_deconn++;
        }
    }
    if (nb_deconn == 0) printf("(aucun)");
    printf("\n");

    free(avant); free(apres);
}

int neutraliserPropagation(grapheReseau g, int idSrc, int idDest) {
    int nb_supprimes = 0, i, j;
    int *parent, *vis;
    LISTE file;

    if (!g || !g->articles[idSrc] || !g->articles[idDest]) return 0;

    while (cheminExiste(g, idSrc, idDest)) {
        /* BFS pour trouver un chemin et reperer un noeud intermediaire */
        parent = (int*)malloc(g->V * sizeof(int));
        vis    = (int*)calloc(g->V, sizeof(int));
        file   = listeCreer();

        for (i = 0; i < g->V; i++) parent[i] = -1;

        vis[idSrc] = 1;
        inserer(file, g->articles[idSrc], 1);

        while (!estVide(file)) {
            ELEMENT c = recuperer(file, 1); supprimer(file, 1);
            if (c->id == idDest) break;
            for (i = 0; i < g->V; i++) {
                if (!g->articles[i] || vis[i]) continue;
                for (j = 1; j <= listeTaille(g->adjList[i]); j++) {
                    ELEMENT e = recuperer(g->adjList[i], j);
                    if (e && e->id == c->id && !vis[i]) {
                        vis[i] = 1;
                        parent[i] = c->id;
                        inserer(file, g->articles[i], listeTaille(file)+1);
                    }
                }
            }
        }
        listeDetruire(file); free(vis);

        /* Remonter le chemin idSrc -> idDest et prendre le premier intermediaire */
        {
            int courant = idDest;
            int intermed = -1;
            while (parent[courant] != -1 && parent[courant] != idSrc) {
                intermed = parent[courant];
                courant  = parent[courant];
            }
            /* Si liaison directe idSrc->idDest sans intermediaire */
            if (intermed == -1) {
                /* On ne peut pas supprimer idSrc ou idDest, rien a faire */
                free(parent);
                break;
            }
            free(parent);
            printf("Article supprime : %s (score:%d)\n",
                   g->articles[intermed]->titre,
                   g->articles[intermed]->score_fiabilite);
            supprimerArticle(g, intermed);
            nb_supprimes++;
        }
    }

    if (g->articles[idSrc] && g->articles[idDest])
        printf("Plus aucun chemin de %s vers %s.\n",
               g->articles[idSrc]->titre, g->articles[idDest]->titre);
    printf("Nombre d'articles supprimes : %d\n", nb_supprimes);
    return nb_supprimes;
}
