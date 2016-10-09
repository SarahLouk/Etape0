//
// Created by user on 07/10/16.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <vector>

#include "terrain.h"
#include "threads.h"

//Information concernant le terrain
#define LONGUEUR 512
#define LARGEUR 128
#define TAILLE_P 4

//Information concernant le mur 1 à céer sur le terrain
int x_mur1 = 16;
int y_mur1_haut = 60;
int y_mur1_bas = 68;
int y_mur = 128;

//Information concernant le mur 2 à céer sur le terrain
int x_mur2 = 112;
int x_mur2_fin = 128;
int y_mur2_haut = 56;
int y_mur2_bas = 72;

int NB_EXEC = 5;
int NB_PERSONNES = 1;
int NB_THREADS = 0;
bool mActivee = false;

//Fin du scénario
bool fin = false;

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

/*
 * Création du terrain
 */
bool** creation_terrain() {
    // Creation de la matrice
    bool **terrain = (bool **) malloc((LARGEUR) * sizeof(bool *));
    for(int i = 0; i < LARGEUR; i++) {
        terrain[i] = (bool *) malloc((LONGUEUR) * sizeof(bool *));
    }

    // Initialisation totale de la matrice a true
    for(int i = 0; i < LARGEUR; i++) {
        for(int j = 0; j < LONGUEUR; j++) {
            terrain[i][j] = true;
        }
    }

    // Creation du mur1
    for(int i = 0; i < x_mur1; i++) {
        for(int j = 0; j < y_mur1_haut; j++) {
            terrain[j][i] = false;
        }
        for(int k = y_mur1_bas; k < y_mur; k++) {
            terrain[k][i] = false;
        }
    }

    // Creation du mur2
    for(int i = x_mur2; i < x_mur2_fin; i++) {
        for(int j = 0; j < y_mur2_haut; j++) {
            terrain[j][i] = false;
        }
        for(int k = y_mur2_bas; k < y_mur; k++) {
            terrain[k][i] = false;
        }
    }

    return terrain;
}

/*
 * Affichage du terrain
 */
void afficher_matrice(bool **terrain) {
    for(int i = 0; i < LARGEUR; i++) {
        for(int j = 0; j < LONGUEUR; j++) {
            cout << terrain[i][j] << " ";
        }
        cout << endl;
    }
}

/*
 * Une personne est représenté par son pixel en haut à gauche (x,y) sont ses coordonnées
 * On regarde si on peut poser une personne à cet endroit
 * (cad qu'il n'existe rien sur la place qu'occuperait une personne)
 */
bool isFree(bool **terrain,int x, int y){
    for(int i = x-(TAILLE_P-1); i <= x; i++){
        for(int j = y-(TAILLE_P-1); j <= y; j++){
            if(!terrain[j][i]) return false;
        }
    }
    return true;
}

/*
 * Une personne est représenté par son pixel en haut à gauche (x,y) sont ses coordonnées
 * On regarde si il y a un mur sur la place qu'occuperait la personne si on decidait de la creer en x;y
 */
bool isNotAWall(int x, int y){
    for(int i=x-(TAILLE_P-1);i<=x;i++){
        for(int j=y-(TAILLE_P-1);j<=y;j++){
            if( (i<x_mur1 && (j<y_mur1_haut || j>y_mur1_bas)) ||
                (i>x_mur2 && i<x_mur2_fin && (j<y_mur2_haut || j>y_mur2_bas))) return false;
        }
    }

    return true;
}

/*
 * On creer une foule de taille 2**p sur le terrain
 */
vector<Personne> init_personnes(bool **terrain, int p){
    vector<Personne> tab_personnes;
    //Pour chaque personne à créer on va choisir aléatoirement un x et y qui se situeront à l'est des murs
    //et où il est possible de créer une personne à moins que quelqu'un ne soit déja là
    for(int i = 0; i < pow(2, p); i++){
        int x = LARGEUR + (TAILLE_P-1) + (rand() % (LONGUEUR - y_mur ));
        int y = rand() % (LARGEUR - (TAILLE_P-1));
        cout << "randoms crees" << endl;
        //On regarde si la place est libre pour les x;y pris aléatoirement puis oncrée la personne sur le terrain
        if(isFree(terrain, x, y)){
            cout << "entrer dans la boucle" << endl;
            //On crée la personne
            Personne p;
            p._x = x;
            p._y = y;
            cout << "creation personne" << endl;
            //On stock la personne
            tab_personnes.push_back(p);
            cout << "stocker personne" << endl;
            //On declare la place de la personne comme occupé maintenant
            for(int i = x-(TAILLE_P-1); i <= x; i++){
                for(int j = y-(TAILLE_P-1); j <= y; j++){
                    terrain[j][i] = false;
                }
            }
        }

        //Si la place pour les x,y pris est déja occupé alors on refait un tour de plus
        else i--;
    }

    return tab_personnes;
}

/*
 * Calcul l'azimuth du coin haut droit d'une personne
 * Revient à calculer l'hypothénuse du triangle rectangle entre (0;LARGEUR/2), (x;y) et (x;y-LARGEUR/2)
 * La ligne LARGEUR/2 est "la distance la plus courte" pour tout y pour arriver à l'arrivé
 */
float azimuth(int x,int y) {
    return (float) sqrt(pow(x, 2) + pow(y - (LARGEUR / 2), 2));
}

/*
 * Calcul la direction qui rend le chemin le plus court pour une personne
 * Retourne 0 si le meilleur mouvement est au Nord, 1 si au Nord Ouest, 2 si à l'Ouest, 3 si au Sud, 4 si au Sud
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
 * Deplace la personne sur le terrain
 * Set a false les nouveaux pixels occupés et true les pixels libérés
 * pour la direction : 0 represente le Nord, 1 le Nord Ouest, 2 l'Ouest, 3 le Sud Ouest et 4 le Sud
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


/*
 * Gère le déplacement et arrivé d'une personne
 * Avance dans la meilleure direction possible sinon attend que la place se libère
 * Pour les deplacements diagonaux, si jamais un mur l'empeche d'avancer alors il se deplace verticalement (meilleur coup possible)
 * On considère qu'une personne est arrivé lorsqu'au moins 1 pixel à dépasser la colonne x=0
 */
void deplacement(bool **terrain, vector<Personne> tab_personnes, int i){
    int dir = meilleur_coup(tab_personnes[i]);
    switch (dir){
        //N
        case 0:
            if(isFree(terrain,tab_personnes[i]._x,tab_personnes[i]._y-1)) actualise(terrain,tab_personnes[i],0);
            break;

            //NO
        case 1:
            if(isFree(terrain,tab_personnes[i]._x-1,tab_personnes[i]._y-1)) actualise(terrain,tab_personnes[i],1);
            else if(!isNotAWall(tab_personnes[i]._x-1,tab_personnes[i]._y-1) && isFree(terrain,tab_personnes[i]._x,tab_personnes[i]._y-1)) actualise(terrain,tab_personnes[i],0);
            break;

            //O
        case 2:
            if(isFree(terrain,tab_personnes[i]._x-1,tab_personnes[i]._y)) actualise(terrain,tab_personnes[i],2);
            else if(tab_personnes[i]._x==3){
                tab_personnes.erase(tab_personnes.begin()+(i-1));
                free(terrain,tab_personnes[i]._x,tab_personnes[i]._y);
            }
            break;

            //SO
        case 3:
            if(isFree(terrain,tab_personnes[i]._x-1,tab_personnes[i]._y+1)) actualise(terrain,tab_personnes[i],3);
            else if(!isNotAWall(tab_personnes[i]._x-1,tab_personnes[i]._y+1) && isFree(terrain,tab_personnes[i]._x,tab_personnes[i]._y+1)) actualise(terrain,tab_personnes[i],4);
            break;

            //S
        case 4:
            if(isFree(terrain,tab_personnes[i]._x,tab_personnes[i]._y+1)) actualise(terrain,tab_personnes[i],4);
            break;
    }

}

/*
 * Libère la place occuppé par la personne qui vient de franchir la ligne d'arrivée
 */
void free(bool **terrain,int x, int y){
    for(int i=x-(TAILLE_P-1);i<=x;i++){
        for(int j=y-(TAILLE_P-1);j<=y;j++){
            terrain[i][j]=true;
        }
    }
}

/*
 * Check si tout le monde est arrivé ou non
 */
bool finScenario(vector<Personne> tab_personnes){
    if(tab_personnes.size()==0) {
        fin = true;
        return true;
    }
    return false;
}


/**
    Lance les scenarios
*/
void executer (int n_personnes, int n_thread) {
    cout << "--------------------------------------------------" << endl;
    cout << "Lancement du programme avec les options suivantes:\nNombre de personnes = " << pow(2, n_personnes) << "\nEtape de threads = " << n_thread << "\nOption m activee? " << mActivee << endl;
    cout << "--------------------------------------------------" << endl;
    double tempsExecCPU[NB_EXEC];   // Tableau contenant les valeurs des temps d'execution CPU
    double tempsExecUser[NB_EXEC];  // Tableau contenant les valeurs des temps d'execution utilisateur
    int i;
    for (i = 1; i <= NB_EXEC; ++i) {    // Boucle pour lancer le bon nombre d'executions
        // On doit creer la matrice ici
        bool **terrain = creation_terrain();
        cout << "Terrain cree avec succes" << endl;
        // On procede a l'initialisation des personnes
        vector<Personne> tab_personnes = init_personnes(terrain, n_personnes);
        cout << "Initialisation avec succes" << endl;
        clock_t chronoCPU;
        time_t chronoUtil;
        //Si m activé on lance le chrono
        if (mActivee) {
            chronoCPU = clock();
            chronoUtil = time(NULL);
            cout << "Lancement du chrono" << endl;
        }
        // Ici on est censee lancer le deplacement donc l'etape 0
        while(!finScenario(tab_personnes)){
            for(int i=0;i<tab_personnes.size();i++){
                deplacement(terrain,tab_personnes,i);
            }
        }

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
    int opt;
    while ((opt = getopt(argc, argv, "t:p:m")) != -1) { // Boucle pour lire les options
        switch (opt) {
            case 'p':   // Option pour le nombre de personnes a executer
                if(is_number(optarg)) { // On verifie que le parametre est bien un nombre
                    int argOption = atoi(optarg);
                    if (argOption >= 0 && argOption < 10) {
                        NB_PERSONNES = argOption; // Nombre d'iterations souhaite
                    } else {
                        fprintf(stderr, "Un nombre superieur ou egal a 0 et inferieur ou egal a 9 est attendu pour l'option p\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    fprintf(stderr, "Un nombre est attendu pour l'option p\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't': // Option pour le nombre de threads a lancer
                if (atoi(optarg) > 2 ||  atoi(optarg) < 0) {
                    fprintf(stderr, "Trop de valeurs pour l'option t %d\n", atoi(optarg));
                    fprintf(stderr, "Un nombre entre 0 et 2 est attendu pour l'option t\n");
                    exit(EXIT_FAILURE);
                } else {
                    NB_THREADS = atoi(optarg);
                }
                break;
            case 'm':   // Option activant la mesure de la consomamation du CPU et du temps utilisateur d'execution
                mActivee = true;
                break;
            default: // Aucune option valable n'a ete reconnue, donc erreur
                fprintf(stderr, "Cette option n'existe pas: %d\n", opt);
                exit(EXIT_FAILURE);
        }
    }
}


int main(int argc, char *argv[]) {
    //get_options(argc, argv); // Recupere les options du programme
    executer(NB_PERSONNES, NB_THREADS);
    //bool ** terrain = creation_terrain();
    //afficher_matrice(terrain);

}