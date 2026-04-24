#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LSTPRIM.H"
#include "GRAPHE.H"


static void viderBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void afficherMenu(void) {
    printf("\n===== Reseau d'Information =====\n");
    printf(" 1. Charger un reseau depuis un fichier\n");
    printf(" 2. Ajouter un article\n");
    printf(" 3. Ajouter une citation\n");
    printf(" 4. Supprimer un article\n");
    printf(" 5. Supprimer une citation\n");
    printf(" 6. Afficher le reseau\n");
    printf(" 7. Articles cites par un article\n");
    printf(" 8. Articles qui citent un article\n");
    printf(" 9. Sources originales / articles isoles\n");
    printf("10. Article le plus cite\n");
    printf("11. Trier par date de publication\n");
    printf("12. Premier article citant\n");
    printf("13. Chaine de propagation\n");
    printf("14. Simuler la propagation (BFS)\n");
    printf("15. Articles accessibles depuis un article\n");
    printf("16. Analyser les articles (fake news)\n");
    printf("17. Articles suspects les plus cites\n");
    printf("18. [BONUS] Simuler la suppression d'un article\n");
    printf("19. [BONUS] Neutraliser une propagation\n");
    printf(" 0. Quitter\n");
    printf("==================================\n");
    printf("Votre choix : ");
}

int main(void) {
    grapheReseau g = NULL;
    int choix;

    do {
        afficherMenu();
        if (scanf("%d", &choix) != 1) { viderBuffer(); continue; }

        viderBuffer();

        switch (choix) {

        case 1: {
            char filename[256];
            printf("Nom du fichier : ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';
            if (g) detruireGraphe(g);
            g = chargerGraphe(filename);
            if (g) printf("Reseau charge avec succes (%d articles).\n", g->V);
            break;
        }

        case 2: {
    ELEMENT art;
    if (!g) { printf("Aucun reseau charge.\n"); break; }
    art = elementCreer();
    if (!art) break;
    printf("--- Saisie d'un article ---\n");
    elementLire(&art);


    if (ajouterArticle(g, art)) {
        printf("Article ajoute.\n");
    } else {
        printf("Echec (ID deja utilise ou invalide).\n");
        elementDetruire(art);
    }
    break;
}

        case 3: {
            int src, dest, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID article source      : ");
                if (scanf("%d", &src) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            do {
                valid = 1;
                printf("ID article destination : ");
                if (scanf("%d", &dest) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            if (ajouterCitation(g, src, dest)) printf("Citation ajoutee.\n");
            else printf("Echec.\n");
            break;
        }

        case 4: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article a supprimer : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            if (supprimerArticle(g, id)) printf("Article supprime.\n");
            else printf("Echec (article introuvable).\n");
            break;
        }

        case 5: {
            int src, dest, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID article source      : ");
                if (scanf("%d", &src) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            do {
                valid = 1;
                printf("ID article destination : ");
                if (scanf("%d", &dest) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            if (supprimerCitation(g, src, dest)) printf("Citation supprimee.\n");
            else printf("Citation introuvable.\n");
            break;
        }

        case 6:
            if (!g) { printf("Aucun reseau charge.\n"); break; }
            afficherGraphe(g);
            break;

        case 7: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            articlesCites(g, id);
            break;
        }

        case 8: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            articlesCitants(g, id);
            break;
        }

        case 9:
            if (!g) { printf("Aucun reseau charge.\n"); break; }
            printf("--- Sources originales ---\n");
            sourcesOriginales(g);
            printf("--- Articles isoles ---\n");
            articlesIsoles(g);
            break;

        case 10:
            if (!g) { printf("Aucun reseau charge.\n"); break; }
            articlePlusCite(g);
            break;

        case 11:
            if (!g) { printf("Aucun reseau charge.\n"); break; }
            trierParDate(g);
            break;

        case 12: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            premierCitant(g, id);
            break;
        }

        case 13: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article source : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            chainePropagation(g, id);
            break;
        }

        case 14: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article source : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            simulerPropagation(g, id);
            break;
        }

        case 15: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article source : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            articlesAccessibles(g, id);
            break;
        }

        case 16:
            if (!g) { printf("Aucun reseau charge.\n"); break; }
            analyserReseau(g);
            break;

        case 17:
            if (!g) { printf("Aucun reseau charge.\n"); break; }
            articlesSuspectsCites(g);
            break;

        case 18: {
            int id, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID de l'article : ");
                if (scanf("%d", &id) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            simulerSuppression(g, id);
            break;
        }

        case 19: {
            int src, dest, valid;
            if (!g) { printf("Aucun reseau charge.\n"); break; }

            do {
                valid = 1;
                printf("ID source      : ");
                if (scanf("%d", &src) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            do {
                valid = 1;
                printf("ID destination : ");
                if (scanf("%d", &dest) != 1) {
                    printf("  Erreur : doit etre un nombre\n");
                    viderBuffer();
                    valid = 0;
                } else {
                    viderBuffer();
                }
            } while (!valid);

            neutraliserPropagation(g, src, dest);
            break;
        }

        case 0:
            printf("Au revoir !\n");
            break;

        default:
            printf("Choix invalide.\n");
        }

    } while (choix != 0);

    if (g) detruireGraphe(g);
    return 0;
}
