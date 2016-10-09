//
// Created by user on 09/10/16.
//

#include "threads.h"


void init_param_thread(Thread_Param* tp, vector<Personne> p, pthread_barrier_t* barriere, pthread_barrier_t* barriereAll) {
    tp->personnes = p;
    tp->barriere_thread = barriere;
    tp->barriere_all = barriereAll;
}

