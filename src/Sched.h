#ifndef SCHED_H
#define SCHED_H

#include <iostream>
#include <cstring>

enum State { RUNNING, READY, BLOCKED, DEAD };

struct TCB {
    int task_id;
    char task_name[64];
    State state;
    TCB* next;
};

class Scheduler {
private:
    TCB* head;
    TCB* current;
    int next_id;

public:
    Scheduler();
    int create_task(const char* name);
    void kill_task(int id);
    void yield();
    void garbage_collect();
    void dump(int level);
    TCB* get_current();
};

#endif