//
// Created by user on 07/10/16.
//

#include <stdlib.h>
#include "terrain.h"
#include <math.h>

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

bool** creation_terrain() {
    // Creation de la matrice
    bool **terrain = (bool **) malloc((LONGUEUR+1) * sizeof(bool *));
    for(int i = 0; i < LONGUEUR; i++) {
        terrain[i] = (bool *) malloc((LARGEUR+1) * sizeof(bool *));
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


personne * init_personnes(bool **terrain, int p){
    personne* tab_personnes= (personne *) malloc(sizeof(personne) * pow(2,p)+1);
    for(int i=0;i<pow(2,p);i++){
        int x=LONGUEUR + rand() % (LONGUEUR-y_mur + (TAILLE_P-1));
        int y=rand() % (LARGEUR - (TAILLE_P-1));

        if(isFree(terrain,x,y)){
            tab_personnes[i]= personne(x,y);
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

float azimuth(personne p) {
    return (float) sqrt(pow(p.x, 2) + pow(p.y - (LARGEUR / 2), 2));
}

/*
 * Retourne 0 si le meilleur mouvement est au Nord, 1 si au NO, 2 si à l'O, 3 si au Su, 4 si au S
 */
int meilleur_coup(personne p){
    int min =LONGUEUR;
    int c;
    //Nord
    if(p.y>0 && min> azimuth(personne(p.x,p.y-1))){
        min = azimuth(personne(p.x,p.y-1));
        c=0;
    }
    //Nord Ouest
    if(p.x>0 && p.y>0 && min> azimuth(personne(p.x-1,p.y-1))){
        min = azimuth(personne(p.x-1,p.y-1));
        c=1;
    }
    //Ouest
    if(p.x>0 && min> azimuth(personne(p.x-1,p.y))) {
        min = azimuth(personne(p.x-1,p.y));
        c=2;
    }
    //Sud Ouest
    if(p.y<LARGEUR && p.x>0 && min> azimuth(personne(p.x-1,p.y+1))){
        min = azimuth(personne(p.x-1,p.y+1));
        c=3;
    }
    //Sud
    if(p.y<LARGEUR && min> azimuth(personne(p.x,p.y+1))) {
        min = azimuth(personne(p.x,p.y+1));
        c=4;
    }

    return c;
}


/*
 * Argument personne peut etre pas bon mieux vaudrait tableau personne avec index de la personne
 * sinon modif sur personne seront perdu
 * A mouins que retourne une personne
 */
personne actualise(bool **terrain, personne p,int dir){
    switch (dir){
        case 0:
            p = personne(p.x,p.y-1);
            for(int i=0;i<TAILLE_P;i++){
                terrain[p.x-i][p.y] = false;
                terrain[p.x-i][p.y+TAILLE_P] = true;
            }
            break;

        case 1:
            p = personne(p.x-1,p.y-1);
            for(int i=0;i<TAILLE_P;i++){
                terrain[p.x-i][p.y] = false;
                terrain[p.x-(i-1)][p.y+TAILLE_P] = true;
            }
            for(int i=1;i<TAILLE_P;i++){
                terrain[p.x-(TAILLE_P-1)][p.y+i] = false;
                terrain[p.x+1][p.y+i] = true;
            }
            break;

        case 2:
            p = personne(p.x-1,p.y);
            for(int i=0;i<TAILLE_P;i++){
                terrain[p.x-(TAILLE_P-1)][p.y+i] = false;
                terrain[p.x+1][p.y+i] = true;
            }
            break;

        case 3:
            p = personne(p.x-1,p.y+1);
            for(int i=0;i<TAILLE_P;i++){
                terrain[p.x-(i-1)][p.y+1] = false;
                terrain[p.x-i][p.y+(TAILLE_P-1)] = true;
            }
            for(int i=0;i<TAILLE_P-1;i++){
                terrain[p.x+1][p.y+i] = false;
                terrain[p.x-(TAILLE_P-1)][p.y+i] = true;
            }
            break;

        case 4:
            p = personne(p.x,p.y+1);
            for(int i=0;i<TAILLE_P;i++){
                terrain[p.x-i][p.y+(TAILLE_P-1)] = false;
                terrain[p.x-i][p.y-1] = true;
            }
            break;
    }

    return p;
}


void deplacement(bool **terrain, personne p){
    int dir = meilleur_coup(p);
    switch (dir){
        case 0:
            if(isFree(terrain,p.x,p.y-1)) actualise(terrain,p,0);
            break;

        case 1:
            if(isFree(terrain,p.x-1,p.y-1)) actualise(terrain,p,1);
            else if(!isNotAWall(p.x-1,p.y-1)) actualise(terrain,p,0);
            break;

        case 2:
            if(isFree(terrain,p.x-1,p.y)) actualise(terrain,p,2);
            break;

        case 3:
            if(isFree(terrain,p.x-1,p.y+1)) actualise(terrain,p,3);
            else if(!isNotAWall(p.x-1,p.y+11)) actualise(terrain,p,4);
            break;

        case 4:
            if(isFree(terrain,p.x,p.y+1)) actualise(terrain,p,4);
            break;
    }

}


int main() {
    bool ** terrain = creation_terrain();
    afficher_matrice(terrain);
}
