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
} Personne;


bool** creation_terrain();
void afficher_matrice(bool ** terrain);
void init_personnes(bool** terrain ,int p);

#endif //ETAPE0_TERRAIN_H
