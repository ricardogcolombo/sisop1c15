#include <vector>
#include <queue>
#include <iostream>
#include "sched_sjf.h"

using namespace std;

SchedSJF::SchedSJF(vector<int> argn) {
	// SJF recibe la cantidad de cores.
	cores = argn[0];
	for(int i = 1; i < (int)argn.size(); i++){
		execution_time.push_back(argn[i]);
	}
}

SchedSJF::~SchedSJF() {
}

void SchedSJF::load(int pid) {
	q.push(make_pair(execution_time[pid],pid)); // llegó una tarea nueva
}

void SchedSJF::unblock(int pid) {
}

int SchedSJF::tick(int cpu, const enum Motivo m) {
	if (m == EXIT) {
		// Si el pid actual terminó, sigue el próximo.
		if (q.empty()) return IDLE_TASK;
		else {
			int sig = q.top().second; q.pop();
			return sig;
		}
	} else {
		// Siempre sigue el pid actual mientras no termine.
		if (current_pid(cpu) == IDLE_TASK && !q.empty()) {
			int sig = q.top().second; q.pop();
			return sig;
		} else {
			return current_pid(cpu);
		}
	}	
}
