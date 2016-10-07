//
// Created by user on 07/10/16.
//

#ifndef ETAPE0_TERRAIN_H
#define ETAPE0_TERRAIN_H

#include <iostream>

using namespace std;

typedef struct {
    int x;
    int y;
} personne;



bool** creation_terrain();
void afficher_matrice(bool ** terrain);
bool isFree(bool **terrain,int x, int y);
bool isNotAWall(int x, int y);
personne* init_personnes(bool** terrain ,int p);
float azimuth(personne p);
int meilleur_coup(personne p);
void deplacement(bool **terrain, personne p);
personne actualise(bool **terrain, personne p,int dir);

#endif //ETAPE0_TERRAIN_H
