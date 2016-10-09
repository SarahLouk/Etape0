//
// Created by user on 07/10/16.
//

#ifndef ETAPE0_TERRAIN_H
#define ETAPE0_TERRAIN_H

#include <iostream>

using namespace std;

typedef struct {
    int _x;
    int _y;
} Personne;

bool** creation_terrain();
void afficher_matrice(bool ** terrain);
bool isFree(bool **terrain,int x, int y);
bool isNotAWall(int x, int y);
vector<Personne> init_personnes(bool** terrain ,int p);
float azimuth(Personne p);
int meilleur_coup(Personne p);
void deplacement(bool **terrain, vector<Personne> tab_personnes,int index);
void actualise(bool **terrain, Personne *p,int dir);
void free(bool **terrain,int x, int y);

double moyenne(double *tableValues, int nbExecutions);
void lancer_statistiques(double *executionsCPU, double *executionsUtil);


#endif //ETAPE0_TERRAIN_H
