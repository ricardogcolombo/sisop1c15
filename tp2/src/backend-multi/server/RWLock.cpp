#include "RWLock.h"

RWLock :: RWLock() {
	//pthread_rwlock_init(rwlock, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&vcr, NULL);
	pthread_cond_init(&vcw, NULL);
	cantLeyendo = 0;
	escribiendo = false;
	cantEscritoresEsperando = 0;
	cantEscrituras = 0;
	cantLecturasSeguidas = 0;

}

void RWLock :: rlock() {
	//pthread_rwlock_rdlock(rwlock);
	pthread_mutex_lock(&mutex);
	while(escribiendo || cantLecturasSeguidas>=10){
		pthread_cond_wait(&vcr,&mutex);
	}
	cantLeyendo++;
	pthread_mutex_unlock(&mutex);
}

void RWLock :: wlock() {
	//pthread_rwlock_wrlock(rwlock);
	pthread_mutex_lock(&mutex);
	cantEscritoresEsperando++;
	while(escribiendo || cantLeyendo>0){
		pthread_cond_wait(&vcw,&mutex);
	}
	escribiendo = true;
	cantLecturasSeguidas = 0;
	pthread_mutex_unlock(&mutex);
}

void RWLock :: runlock() {
	//pthread_rwlock_unlock(rwlock);
	pthread_mutex_lock(&mutex);
	cantLeyendo--;
	cantLecturasSeguidas++;
	if(cantLeyendo==0 || cantLecturasSeguidas>=10){
		pthread_cond_signal(&vcw);
	}
	pthread_mutex_unlock(&mutex);
}

void RWLock :: wunlock() {
	//pthread_rwlock_unlock(rwlock);
	pthread_mutex_lock(&mutex);
	escribiendo = false;
	cantEscritoresEsperando--;
	cantEscrituras++;
	pthread_cond_broadcast(&vcr);
	pthread_mutex_unlock(&mutex);
}
