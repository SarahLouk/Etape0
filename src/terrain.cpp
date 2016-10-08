//
// Created by user on 07/10/16.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "terrain.h"

#define LONGUEUR 512
#define LARGEUR 128
#define TAILLE_P 4

int x_mur1 = 16;
int y_mur1_haut = 60;
int y_mur1_bas = 68;
int y_mur = 128;

int x_mur2 = 112;
int x_mur2_fin = 128;
int y_mur2_haut = 56;
int y_mur2_bas = 72;

int NB_EXEC = 5;
int NB_PERSONNES = 1;
int NB_THREADS = 0;
bool mActivee = false;

int getopt(int argc, char * const argv[],const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;



/**
    Renvoie la moyenne des valeurs d'un tableau de double, de taille nbExecutions
*/
double moyenne(double *tableValues, int nbExecutions) {
    double sum = 0.0;
    int i;
    for (i = 0; i < nbExecutions; ++i) {
        sum += tableValues[i]; // On fait le total de toutes les valeurs
    }
    return sum/(nbExecutions); // Ici on divise par nbExecutions
}

/**
    Verifie si les calculs doivent etre lances, et les lancer si besoin
*/
void lancer_statistiques(double *executionsCPU, double *executionsUtil) {
    if(mActivee) {
        cout << "\nMoyenne des temps d'execution CPU: " << moyenne(executionsCPU, NB_EXEC) << endl;
        cout << "\nMoyenne des temps de reponses utilisateur: " << moyenne(executionsUtil, NB_EXEC) << endl;
    }
}

bool** creation_terrain() {
    // Creation de la matrice
    bool **terrain = (bool **) malloc((LONGUEUR) * sizeof(bool *));
    for(int i = 0; i < LONGUEUR; i++) {
        terrain[i] = (bool *) malloc((LARGEUR) * sizeof(bool *));
    }

    // Initialisation totale de la matrice a true
    for(int i = 0; i < LONGUEUR; i++) {
        for(int j = 0; j < LARGEUR; j++) {
            terrain[i][j] = true;
        }
    }

    // Creation du mur1
    for(int i = 0; i < x_mur1; i++) {
        for(int j = 0; j < y_mur1_haut; j++) {
            terrain[i][j] = false;
        }
        for(int k = y_mur1_bas; k < y_mur; k++) {
            terrain[i][k] = false;
        }
    }

    // Creation du mur2
    for(int i = x_mur2; i < x_mur2_fin; i++) {
        for(int j = 0; j < y_mur2_haut; j++) {
            terrain[i][j] = false;
        }
        for(int k = y_mur2_bas; k < y_mur; k++) {
            terrain[i][k] = false;
        }
    }

    return terrain;
}


void afficher_matrice(bool **terrain) {
    for(int i = 0; i < LONGUEUR; i++) {
        for(int j = 0; j < LARGEUR; j++) {
            cout << terrain[i][j] << " ";
        }
        cout << endl;
    }
}


bool isFree(bool **terrain,int x, int y){
    for(int i=x-(TAILLE_P-1);i<=x;i++){
        for(int j=y-(TAILLE_P-1);j<=y;j++){
            if(!terrain[i][j]) return false;
        }
    }
    return true;
}

bool isNotAWall(int x, int y){
    for(int i=x-(TAILLE_P-1);i<=x;i++){
        for(int j=y-(TAILLE_P-1);j<=y;j++){
            if( (i<x_mur1 && (j<y_mur1_haut || j>y_mur1_bas)) ||
                (i>x_mur2 && i<x_mur2_fin && (j<y_mur2_haut || j>y_mur2_bas))) return false;
        }
    }

    return true;
}


Personne * init_personnes(bool **terrain, int p){
    Personne* tab_personnes= (Personne *) malloc(sizeof(Personne) * pow(2,p));
    for(int i=0;i<pow(2,p);i++){
        int x=LONGUEUR + rand() % (LONGUEUR-y_mur + (TAILLE_P-1));
        int y=rand() % (LARGEUR - (TAILLE_P-1));

        if(isFree(terrain,x,y)){
            Personne p;
            p._x = x;
            p._y = y;
            tab_personnes[i]= p;
            for(int i=x-(TAILLE_P-1);i<=x;i++){
                for(int j=y-(TAILLE_P-1);j<=y;j++){
                    terrain[i][j] = false;
                }
            }
        }

        else i--;
    }

    return tab_personnes;
}

float azimuth(int x,int y) {
    return (float) sqrt(pow(x, 2) + pow(y - (LARGEUR / 2), 2));
}

/*
 * Retourne 0 si le meilleur mouvement est au Nord, 1 si au NO, 2 si à l'O, 3 si au Su, 4 si au S
 */
int meilleur_coup(Personne p){
    int min =LONGUEUR;
    int c;
    //Nord
    if(p._y>0 && min> azimuth(p._x,p._y-1)){
        min = azimuth(p._x,p._y-1);
        c=0;
    }
    //Nord Ouest
    if(p._x>0 && p._y>0 && min> azimuth(p._x-1,p._y-1)){
        min = azimuth(p._x-1,p._y-1);
        c=1;
    }
    //Ouest
    if(p._x>0 && min> azimuth(p._x-1,p._y)) {
        min = azimuth(p._x-1,p._y);
        c=2;
    }
    //Sud Ouest
    if(p._y<LARGEUR && p._x>0 && min> azimuth(p._x-1,p._y+1)){
        min = azimuth(p._x-1,p._y+1);
        c=3;
    }
    //Sud
    if(p._y<LARGEUR && min> azimuth(p._x,p._y+1)) {
        min = azimuth(p._x,p._y+1);
        c=4;
    }

    return c;
}


/*
 * Argument personne peut etre pas bon mieux vaudrait tableau personne avec index de la personne
 * sinon modif sur personne seront perdu
 * A mouins que retourne une personne
 */
Personne actualise(bool **terrain, Personne p,int dir){
    switch (dir){
        case 0:
            p._y --;
            for(int i=0;i<TAILLE_P;i++){
                terrain[p._x-i][p._y] = false;
                terrain[p._x-i][p._y+TAILLE_P] = true;
            }
            break;

        case 1:
            p._x--;
            p._y--;
            for(int i=0;i<TAILLE_P;i++){
                terrain[p._x-i][p._y] = false;
                terrain[p._x-(i-1)][p._y+TAILLE_P] = true;
            }
            for(int i=1;i<TAILLE_P;i++){
                terrain[p._x-(TAILLE_P-1)][p._y+i] = false;
                terrain[p._x+1][p._y+i] = true;
            }
            break;

        case 2:
            p._x--;
            for(int i=0;i<TAILLE_P;i++){
                terrain[p._x-(TAILLE_P-1)][p._y+i] = false;
                terrain[p._x+1][p._y+i] = true;
            }
            break;

        case 3:
            p._x--;
            p._y++;
            for(int i=0;i<TAILLE_P;i++){
                terrain[p._x-(i-1)][p._y+1] = false;
                terrain[p._x-i][p._y+(TAILLE_P-1)] = true;
            }
            for(int i=0;i<TAILLE_P-1;i++){
                terrain[p._x+1][p._y+i] = false;
                terrain[p._x-(TAILLE_P-1)][p._y+i] = true;
            }
            break;

        case 4:
            p._y++;
            for(int i=0;i<TAILLE_P;i++){
                terrain[p._x-i][p._y+(TAILLE_P-1)] = false;
                terrain[p._x-i][p._y-1] = true;
            }
            break;
    }

    return p;
}


void deplacement(bool **terrain, Personne p){
    int dir = meilleur_coup(p);
    switch (dir){
        //N
        case 0:
            if(isFree(terrain,p._x,p._y-1)) actualise(terrain,p,0);
            //else if wait;
            break;

        //NO
        case 1:
            if(isFree(terrain,p._x-1,p._y-1)) actualise(terrain,p,1);
            else if(!isNotAWall(p._x-1,p._y-1)) actualise(terrain,p,0);
            break;

        //O
        case 2:
            if(isFree(terrain,p._x-1,p._y)) actualise(terrain,p,2);
            break;

        //SO
        case 3:
            if(isFree(terrain,p._x-1,p._y+1)) actualise(terrain,p,3);
            else if(!isNotAWall(p._x-1,p._y+11)) actualise(terrain,p,4);
            break;

        //S
        case 4:
            if(isFree(terrain,p._x,p._y+1)) actualise(terrain,p,4);
            break;
    }

}

/**
    Lance les scenarios
*/
void executer (int n_personnes, int n_thread) {
    cout << "--------------------------------------------------" << endl;
    cout << "Lancement du programme avec les options suivantes:\nNombre de personnes = " << n_personnes << "\nNombre de threads = " << n_thread << "\nOption m activee? " << mActivee << endl;
    cout << "--------------------------------------------------" << endl;
    double tempsExecCPU[NB_EXEC];   // Tableau contenant les valeurs des temps d'execution CPU
    double tempsExecUser[NB_EXEC];  // Tableau contenant les valeurs des temps d'execution utilisateur
    int i;
    for (i = 1; i <= NB_EXEC; ++i) {    // Boucle pour lancer le bon nombre d'executions
        // On doit creer la matrice ici
        // On procede a l'initialisation du terrain
        clock_t chronoCPU;
        time_t chronoUtil;
        if (mActivee) {
            chronoCPU = clock();
            chronoUtil = time(NULL);
        }
        // Ici on est censee lancer le deplacement donc l'etape 0
        if (mActivee) { // Si option m activee
            chronoCPU = clock() - chronoCPU; // On recupere le temps CPU ecoule pour cette execution
            tempsExecCPU[i] = (double) chronoCPU/ CLOCKS_PER_SEC; // On place ce temps dans le tableau des mesures de temps CPU
            printf("Temps d'execution CPU : %2f s.\n", tempsExecCPU[i]); // Affichage du temps CPU
            chronoUtil = time(NULL) - chronoUtil; // On recupere le temps utilisateur ecoule
            tempsExecUser[i] = (double) chronoUtil; // On l'insere dans le tableau des mesures de temps utilisateur
            printf("Temps d'execution user : %2f s.\n", tempsExecUser[i]); // Affichage du temps utilisateur

        }
    }
    lancer_statistiques(tempsExecCPU, tempsExecUser); // Lance le traitement des statistiques
}

/**
    Verifie qu'une chaine de caractere est un nombre, utilise pour les options
*/
bool is_number(char *arg) {
    int i = 0;
    while(arg[i] != '\0') {
        if (!isdigit(arg[i])) {
            return false;   // Si on trouve un caractere n'etant pas un chiffre, renvoie false
        }
        i++;
    }
    return true;
}

/**
    Recupere les options passees au programme et modifie les variables globales en fonction de ces options.
*/
void get_options(int argc, char ** argv) {
    int opt, i;
    while ((opt = getopt(argc, argv, "m:p:t:")) != -1) { // Boucle pour lire les options
        switch (opt) {
            case 'p':   // Option pour le nombre d'itérations a executer
                if(is_number(optarg)) { // On verifie que le parametre est bien un nombre
                    int argOption = atoi(optarg);
                    if (argOption >= 0 && argOption < 10) {
                        NB_PERSONNES = argOption; // Nombre d'iterations souhaite
                        printf("Nombre de personnes: %d\n", NB_PERSONNES);
                    } else {
                        fprintf(stderr, "Un nombre superieur ou egal a 0 et inferieur ou egal a 9 est attendu pour l'option p\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    fprintf(stderr, "Un nombre est attendu pour l'option p\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':   // Option activant la mesure de la consomamation du CPU et du temps utilisateur d'execution
                mActivee = true;
                break;
            case 't': // Option pour le nombre de threads a lancer
                NB_THREADS = strlen(optarg);
                if (NB_THREADS > 1) {
                    fprintf(stderr, "Trop de valeurs pour l'option t\n");
                }

                for (i = 0; i < NB_THREADS; i++) {
                    if(isdigit(optarg[i])) {
                        int argOption = optarg[i] - '0'; // Recupere la valeur associee a l'option t
                        if (argOption == 0) {
                            printf("Nombre de threads a executer: 1 \n");
                        } else if (argOption == 1) {
                            printf("Nombre de threads a executer: 4\n");
                        } else if (argOption == 2) {
                            int nb_threads = pow(2, NB_PERSONNES);
                            printf("Nombre de threads a executer: %d\n", nb_threads);
                        } else {    // Cas ou une valeur inferieure a 0 ou superieure a 2 a ete saisie
                            fprintf(stderr, "Un nombre entre 0 et 2 est attendu pour l'option t\n");
                            exit(EXIT_FAILURE);
                        }
                    } else { // Cas ou la valeur associee a l'option n'est pas un chiffre
                        fprintf(stderr, "Un nombre entre 0 et 2 est attendu pour l'option t\n");
                    }
                }
                break;
            default: // Aucune option valable n'a ete reconnue, donc erreur
                fprintf(stderr, "Cette option n'existe pas: %d\n", opt);
                exit(EXIT_FAILURE);
                break;
        }
    }
}


int main() {
    bool ** terrain = creation_terrain();
    afficher_matrice(terrain);
}
