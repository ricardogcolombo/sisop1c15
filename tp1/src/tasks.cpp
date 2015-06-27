#include "tasks.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>	
using namespace std;

void TaskCPU(int pid, vector<int> params) { // params: n
	uso_CPU(pid, params[0]); // Uso el CPU n milisegundos.
}

void TaskIO(int pid, vector<int> params) { // params: ms_pid, ms_io,
	uso_CPU(pid, params[0]); // Uso el CPU ms_pid milisegundos.
	uso_IO(pid, params[1]); // Uso IO ms_io milisegundos.
}

void TaskAlterno(int pid, vector<int> params) { // params: ms_pid, ms_io, ms_pid, ...
	for(int i = 0; i < (int)params.size(); i++) {
		if (i % 2 == 0) uso_CPU(pid, params[i]);
		else uso_IO(pid, params[i]);
	}
}

void TaskConBloqueo(int pid, vector<int> params) {
	int tiempo = params[0];
	int inicioBloq = params[1];
	int finBloq = params[2];

	if(inicioBloq > tiempo){
		uso_CPU(pid, tiempo);
	}else{
		uso_CPU(pid,inicioBloq-2);
		uso_IO(pid,finBloq-inicioBloq+1);
		uso_CPU(pid,tiempo-finBloq);
	}
}
// n bmin y bmax en el vector
void TaskConsola( int pid, vector<int> params) {
	int n =  params[0];
	int bmin = params[1];
	int bmax = params[2]; 
	int intervalo = bmax-bmin+1;
	for (int i = 0; i < params[0]; i++) {
		int tiempoBloq = rand()% intervalo +params[1];
		uso_IO(pid,tiempoBloq);
	}
}

void TaskBatch(int pid, vector<int> params) { // params: total_cpu, cant_bloqueos, ...
	//Programar un tipo de tarea TaskBatch que reciba dos par ́ametros: total cpu y cant bloqueos. 
	//Una tarea de este tipo deber ́a realizar cant bloqueos llamadas bloqueantes, en momentos elegidos pseudoaleatoriamente. 
	//En cada tal ocasi ́on, la tarea deber ́a permanecer bloqueada durante exactamente un (1) ciclo de reloj
	int total_cpu = params[0];
	int cant_bloqueos = params[1];
	//creo un arreglo que cada posicion representa un tick de su ejecucion
	int tiempos[total_cpu];
	int bloqueos_seteados = 0;
	//lleno todo de ceros por si las dudas
	//cout << "cantidad de bloqueos " << cant_bloqueos <<endl;
	for(int j =0;j<total_cpu;j++){
		tiempos[j]=0;
	}
	//elijo numeros aleatorios entre 0 y tiempo total para saber poner cuando hay un bloqueo
	while(bloqueos_seteados<cant_bloqueos){
		int tiempo_bloq = rand()% total_cpu-1;
		if(tiempos[tiempo_bloq]==0){
			tiempos[tiempo_bloq]=1;
			bloqueos_seteados++;
		}
	}	
	for(int i =0;i<total_cpu;i++){
		if(tiempos[i]==0){
			uso_CPU(pid,1);
	//cout << "estoy corriendo" << pid;
	//cout << " " << i <<endl;
		}else{
			uso_IO(pid,1);
	//cout << "me bloqueo "<< pid;
	cout << " " << i <<endl;
		}
	}
}


void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
	register_task(TaskConBloqueo,3);
	//definidos por el grupo
	register_task(TaskConsola,3);
	register_task(TaskBatch,2);
	
}

