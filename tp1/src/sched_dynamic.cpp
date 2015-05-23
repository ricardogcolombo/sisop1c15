#include "sched_dynamic.h"
#include <iostream>
#include <list>

using namespace std;

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

priority_queue<task,vector<task>,comparePID> ColaDynamic;
std::list<task> tareasCorriendo;

SchedDynamic::SchedDynamic(vector<int> argn) {

}

SchedDynamic::~SchedDynamic() {
}

void SchedDynamic::initialize() {
}

void SchedDynamic::load(int pid) {
	int tickActual = current_time();
	int periodo = period(pid);
	task t1 = {tickActual+periodo,pid};
	ColaDynamic.push(t1);

}

void SchedDynamic::unblock(int pid) {
}

int SchedDynamic::tick(int cpu, const enum Motivo m) {


	if(m == TICK ){
		if(current_pid(cpu) == IDLE_TASK && ColaDynamic.empty()) return IDLE_TASK;
		if(ColaDynamic.empty()) return current_pid(cpu);
		if(current_pid(cpu) == IDLE_TASK ) {
			task t1 = ColaDynamic.top();
			ColaDynamic.pop();
			tareasCorriendo.push_back(t1);
			return t1.pid;
		}else{
			std::list<task>::iterator it2;

		  	for (std::list<task>::iterator it=tareasCorriendo.begin(); it != tareasCorriendo.end(); ++it){
	  			if(it->pid == current_pid(cpu)){
	  				it2 = it;
	  				break;		  				
	  			}
		  	}

			task t1 = ColaDynamic.top();
		  	if(t1.prioridad < it2->prioridad){
		  		task nueva = {it2->prioridad,it2->pid };
		  		it2 = tareasCorriendo.erase(it2);
		  		tareasCorriendo.push_back(t1);
		  		ColaDynamic.pop();
		  		ColaDynamic.push(nueva);
		 
		  		return t1.pid;
		  	}

		  	return current_pid(cpu);
		}
	}
	if(m == EXIT ){

		for (std::list<task>::iterator it=tareasCorriendo.begin(); it != tareasCorriendo.end(); ++it){
  			if(it->pid == current_pid(cpu)){
  				it = tareasCorriendo.erase(it);
  				break;  				
  			}
		}

		if(ColaDynamic.empty()) return IDLE_TASK;

		task t1 = ColaDynamic.top();
		ColaDynamic.pop();
		tareasCorriendo.push_back(t1);

		return t1.pid;
	}

	return IDLE_TASK;
}
