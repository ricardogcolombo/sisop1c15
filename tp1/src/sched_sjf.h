#ifndef __SCHED_SJF__
#define __SCHED_SJF__

#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"

using namespace std;

class SchedSJF : public SchedBase {
	public:
		SchedSJF(std::vector<int> argn);
        ~SchedSJF();
		virtual void initialize() {};
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);	
	private:
		int cores;
		priority_queue< pair<int,int>, 
			vector< pair<int,int> >,
		   	greater< pair<int,int> > > q;
		vector<int> execution_time;
		vector<int> task_for_cpu;
		void assign();
};

#endif
