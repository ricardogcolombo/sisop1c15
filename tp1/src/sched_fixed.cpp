#include "sched_fixed.h"
#include <iostream>
#include <math.h> 
using namespace std;


int cCores;
int *quantumsFixed;
int *contadores_fixed;
// empty 

struct task{
	int prioridad;
	int pid;
};
class comparePID{
	public:
		bool operator()(task& p1, task& p2){
			return (p2.prioridad<=p1.prioridad);
		}
};

priority_queue<task,vector<task>,comparePID> ColaFixed;

SchedFixed::SchedFixed(vector<int> argn) {

}

SchedFixed::~SchedFixed() {

}

void SchedFixed::initialize() {

}

void SchedFixed::load(int pid) {
	task t1 =  {period(pid), pid};
	// tengo q chequear el deadline para ver si directamente la mando a correr o la pongo en la cola 
	ColaFixed.push(t1);
}

void SchedFixed::unblock(int pid) {
	//ColaFixed.push(pid);
}

int SchedFixed::tick(int cpu, const enum Motivo m) {

	if(m == TICK ){
		if(current_pid(cpu) == IDLE_TASK && ColaFixed.empty()) return IDLE_TASK;
		if(ColaFixed.empty()) return current_pid(cpu);
		if(current_pid(cpu) == IDLE_TASK ) {
			task t1 = ColaFixed.top();
			ColaFixed.pop();
			return t1.pid;
		}else{
			task t1 = ColaFixed.top();
			if(period(current_pid(cpu))<t1.prioridad) return current_pid(cpu);			
			ColaFixed.pop();
			int cupid = current_pid(cpu); 
			task t2 = {period(cupid),cupid};

			ColaFixed.push(t2);
			return t1.pid;
		}
	}
	if(m==BLOCK || m == EXIT ){
		if(ColaFixed.empty()) return IDLE_TASK;
		task t1 = ColaFixed.top();
		ColaFixed.pop();
		return t1.pid;
	}
	return IDLE_TASK;
}
