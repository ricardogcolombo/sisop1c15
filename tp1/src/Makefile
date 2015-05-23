export LC_ALL=C
CXXFLAGS= -Wall -pedantic -g -ggdb
LDFLAGS= -lpthread

OBJS=main.o simu.o basetask.o tasks.o sched_fcfs.o sched_sjf.o sched_rr2.o sched_rr.o sched_fixed.o sched_dynamic.o
MAIN=simusched

.PHONY: all
all: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

basetask.o: basetask.cpp basetask.h
main.o: main.cpp simu.h basetask.h basesched.h tasks.h sched_sjf.h \
 sched_fcfs.h sched_rr2.h sched_rr.o
sched_sjf.o: sched_sjf.cpp sched_sjf.h basesched.h
sched_fcfs.o: sched_fcfs.cpp sched_fcfs.h basesched.h
sched_rr2.o: sched_rr2.cpp sched_rr2.h basesched.h
sched_rr.o: sched_rr.cpp sched_rr.h basesched.h
sched_fixed.o: sched_fixed.cpp sched_fixed.h basesched.h
sched_dynamic.o: sched_dynamic.cpp sched_dynamic.h basesched.h
simu.o: simu.cpp simu.h basetask.h basesched.h
simusched.o: simusched.cpp basetask.h basesched.h tasks.h
tasks.o: tasks.cpp tasks.h basetask.h


.PHONY: clean
clean:
	rm -f $(OBJS) $(MAIN)

.PHONY: new
new: clean all
