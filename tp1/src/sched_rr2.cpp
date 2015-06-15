#include <vector>
#include <queue>
#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>
#include <map>
#include <vector>

using namespace std;

vector <queue<int> > colaProcesador;                 // empty
map<int,int> procesadorAsignado;
int cantCoresRR2;
int *quantumsRR2;
int *contadores_quantumRR2;
int *cantidadDeProcesosTotalCPU;

SchedRR2::SchedRR2(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cantCoresRR2 = argn[0];
	quantumsRR2 = new int[cantCoresRR2];
	contadores_quantumRR2 =  new int[cantCoresRR2];
	cantidadDeProcesosTotalCPU = new int[cantCoresRR2];

	colaProcesador.resize(cantCoresRR2);

	for (int i = 0; i < cantCoresRR2; i++)
	{
		cantidadDeProcesosTotalCPU[i] = 0;
		contadores_quantumRR2[i] = 0;
		quantumsRR2[i] = argn[i+1];
	}
}

SchedRR2::~SchedRR2() {
	delete(quantumsRR2);
	delete(contadores_quantumRR2);
	delete(cantidadDeProcesosTotalCPU);
}


void SchedRR2::load(int pid) 
{
	int cantidadDeProcesosMenor = 100000;
	int cpuMenor = 0;

	for(int i = 0; i < cantCoresRR2; i++)
	{
		if(cantidadDeProcesosTotalCPU[i] < cantidadDeProcesosMenor) {
			cpuMenor = i;
			cantidadDeProcesosMenor = cantidadDeProcesosTotalCPU[i] ;

		}
	}
	cantidadDeProcesosTotalCPU[cpuMenor]++;
	colaProcesador[cpuMenor].push(pid);
	procesadorAsignado[pid] = cpuMenor;
}

void SchedRR2::unblock(int pid) 
{
	colaProcesador[procesadorAsignado[pid]].push(pid);
}

int SchedRR2::tick(int cpu, const enum Motivo m) {


	int current_process = current_pid(cpu);

	contadores_quantumRR2[cpu]++;

	if(m == TICK ){
		if(current_pid(cpu) == IDLE_TASK && colaProcesador[cpu].empty()) return IDLE_TASK;
		if(current_pid(cpu) == IDLE_TASK && !colaProcesador[cpu].empty()) return next(cpu);

		if( contadores_quantumRR2[cpu]==quantumsRR2[cpu]){
			colaProcesador[cpu].push(current_process);
			return next(cpu);
		}
	}
	if(m==BLOCK || m == EXIT ){
		if(m == EXIT) cantidadDeProcesosTotalCPU[cpu]--;
		return next(cpu);
	}
	return current_pid(cpu);
}

int SchedRR2::next(int cpu) {
	if(colaProcesador[cpu].empty()){
		return IDLE_TASK;
	}
	int pid = colaProcesador[cpu].front();
	colaProcesador[cpu].pop();
	contadores_quantumRR2[cpu]=0;
	return pid;
}
