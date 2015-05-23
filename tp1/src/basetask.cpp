#include "basetask.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

#define esta(e, X) ((X).find(e) != (X).end())
map<string, ptski> task_defs;
vector<TaskBase*> tasks;

vector<ptsk> tasks_load(const char* filename) {
	vector<ptsk> ts(0);
	ifstream f(filename);
	string s; int l = 0;
	unsigned int starttm = 0;
	while(getline(f, s)) { l++;
		if (s == "" || s[0] == '#') continue;
		istringstream iss(s);
		if (s[0] == '@') {
			char c; iss >> c;
			iss >> starttm;
			continue;
		}
		int times = 1;
		int period = 0;
		char type = ' ';
		int cputime = 0;
		if(s[0] == '&') {
			char c; iss >> c;
			iss >> type;
			iss >> times;
			iss >> c;
			if(c != ','){ cerr << filename << ":" << l << ": ERROR: Unexpected character " << c << endl; exit(1); }
			iss >> period;
			iss >> c;
			if(c != ','){ cerr << filename << ":" << l << ": ERROR: Unexpected character " << c << endl; exit(1); }
			iss >> cputime;
			if(period <= 0 or times <=0 or cputime <= 0){ cerr << "WARNING: '&Tn,t,c' should have three positive numbers n, t and c." << endl; period = 0; times = 0; }
		}
		else if (s[0] == '*') {
			char c; iss >> c;
			iss >> times;
			if (times<=0) { cerr << "WARNING: '*n' should have a positive number n." << endl; times = 1; }
		}
		if(period > 0) {
			string nom = "TaskCPU";
			if(!esta(nom, task_defs)) {
				cerr << filename << ":" << l << ": ERROR: Unknow task type (" << nom << "): " << s << endl;
				exit(1);
			}
			ptski ti = task_defs[nom];
			vector<int> params;
			params.push_back(cputime);
			if (ti.second != -1 && ti.second != (int)params.size()) {
				cerr << filename << ":" << l << ": ERROR: expected " << ti.second << " parameters but " << params.size() << " found: " << s << endl;
				exit(1);
			}
			for(int j=0; j<times; j++) ts.push_back(ptsk(ti.first, params, starttm+j*period, type, period, cputime));
		}
		else {
			string nom = "";
			vector<int> params; int x;
			if (!(iss >> nom) || !esta(nom, task_defs)) {
				cerr << filename << ":" << l << ": ERROR: Unknow task type (" << nom << "): " << s << endl;
				exit(1);
			}
			while (iss >> x) params.push_back(x);
			ptski ti = task_defs[nom];
			if (ti.second != -1 && ti.second != (int)params.size()) {
				cerr << filename << ":" << l << ": ERROR: expected " << ti.second << " parameters but " << params.size() << " found: " << s << endl;
				exit(1);
			}
			for(int j=0; j<times; j++) ts.push_back(ptsk(ti.first, params, starttm, type));
		}
	}
	return ts;
}

ptsk::ptsk(TaskBase* vtsk, const std::vector<int>& vprms, unsigned int vstart, char vtype) : tsk(vtsk), prms(vprms), start(vstart), type(vtype), period(0), cputime(0) {}
ptsk::ptsk(TaskBase* vtsk, const std::vector<int>& vprms, unsigned int vstart, char vtype, unsigned int vperiod, unsigned int vcputime) : tsk(vtsk), prms(vprms), start(vstart), type(vtype), period(vperiod), cputime(vcputime) {}
ptsk::ptsk(void) {}
