//
// Created by user on 07/10/16.
//

#include <stdlib.h>
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
    cout << "FIN" << terrain[0][126] << endl;
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
int main() {
    bool ** terrain = creation_terrain();
    afficher_matrice(terrain);
}