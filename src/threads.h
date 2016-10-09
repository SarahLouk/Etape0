//
// Created by user on 09/10/16.
//

#ifndef ETAPE0_THREADS_H
#define ETAPE0_THREADS_H

#include <pthread.h>

typedef struct {
    vector<Personne> personnes;
    pthread_barrier_t* barriere_thread;
    pthread_barrier_t* barriere_all;
} Thread_Param;

void init_param_thread(Thread_Param* tp, vector<Personne> p, pthread_barrier_t* barriere, pthread_barrier_t* barriereAll);


#endif //ETAPE0_THREADS_H
