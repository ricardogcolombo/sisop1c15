#include <vector>

#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

std::queue<int> ColaGlobal;                 // empty 
int cantCores;
int *quantums;
int *contadores_quantum;

//Ejemplo de como se usa
//./simusched tsks/ejercicio2.tsk 1 1 1 SchedRR 5

SchedRR::SchedRR(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cantCores = argn[0];
	quantums = new int[cantCores];
	contadores_quantum =  new int[cantCores];
	for (int i = 0; i < cantCores; i++)
	{
		contadores_quantum[i] = 0;
		quantums[i] = argn[i+1];
	}
}

SchedRR::~SchedRR() {
}



void SchedRR::load(int pid) {
	ColaGlobal.push(pid);
}

void SchedRR::unblock(int pid) {
	ColaGlobal.push(pid);
}

int SchedRR::tick(int cpu, const enum Motivo m) {

	contadores_quantum[cpu]++;


	if(m == TICK ){
		if(current_pid(cpu) == IDLE_TASK && ColaGlobal.empty()) return IDLE_TASK;
		if(current_pid(cpu) == IDLE_TASK && !ColaGlobal.empty()) return next(cpu);
		 
		if( contadores_quantum[cpu]==quantums[cpu]){

			ColaGlobal.push(current_pid(cpu));
			//int pid = ColaGlobal.front();
			//ColaGlobal.pop();
			//contadores_quantum[cpu-1]=0;
			//return pid;
			return next(cpu);
		}
	}
	if(m==BLOCK || m == EXIT ){
			
			//int pid = ColaGlobal.front();
			//ColaGlobal.pop();
			//contadores_quantum[cpu-1]=0;
			//return pid;
			return next(cpu);
	}
	return current_pid(cpu);
}

int SchedRR::next(int cpu) {
	if(ColaGlobal.empty()){
		return IDLE_TASK;
	}
	int pid = ColaGlobal.front();
	ColaGlobal.pop();
	contadores_quantum[cpu]=0;
	return pid;
}
