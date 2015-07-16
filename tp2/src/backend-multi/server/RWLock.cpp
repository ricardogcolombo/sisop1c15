#include "RWLock.h"

RWLock :: RWLock() {
	//pthread_rwlock_init(rwlock, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&vcr, NULL);
	pthread_cond_init(&vcw, NULL);
	cantLeyendo = 0;
	escribiendo = false;
	cantLecturasSeguidas = 0;
	quieroEscribir = 0;
	esperoParaLeer = 0;
}

void RWLock :: rlock() {
	//pthread_rwlock_rdlock(rwlock);
	pthread_mutex_lock(&mutex);
	esperoParaLeer++;
	while (escribiendo || (cantLecturasSeguidas >= 10 && quieroEscribir != 0 )) {
		pthread_cond_wait(&vcr, &mutex);
	}
	esperoParaLeer--;
	cantLeyendo++;
	pthread_mutex_unlock(&mutex);
}

void RWLock :: wlock() {
	//pthread_rwlock_wrlock(rwlock);
	pthread_mutex_lock(&mutex);
	quieroEscribir++;
	while (escribiendo || cantLeyendo > 0) {
		pthread_cond_wait(&vcw, &mutex);
	}
	quieroEscribir--;
	escribiendo = true;
	cantLecturasSeguidas = 0;
	pthread_mutex_unlock(&mutex);
}

void RWLock :: runlock() {
	//pthread_rwlock_unlock(rwlock);
	pthread_mutex_lock(&mutex);
	cantLeyendo--;
	cantLecturasSeguidas++;
	if (cantLeyendo == 0 || cantLecturasSeguidas >= 10) {
		pthread_cond_signal(&vcw);
	}
	pthread_mutex_unlock(&mutex);
}

void RWLock :: wunlock() {
	//pthread_rwlock_unlock(rwlock);
	pthread_mutex_lock(&mutex);
	escribiendo = false;
	if(esperoParaLeer > 0)
		pthread_cond_broadcast(&vcr);
	else
		pthread_cond_signal(&vcw);
	pthread_mutex_unlock(&mutex);
}
