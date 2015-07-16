#ifndef RWLock_h
#define RWLock_h
#include <iostream>

class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();

    private:
        //pthread_rwlock_t rwlock;
        pthread_mutex_t mutex;
        pthread_cond_t vcr;
        pthread_cond_t vcw;
        int cantLeyendo;
        bool escribiendo;
        int cantLecturasSeguidas;
        int esperoParaLeer;
        int quieroEscribir;
};

#endif
