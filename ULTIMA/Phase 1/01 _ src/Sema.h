#ifndef SEMA_H
#define SEMA_H

#include <queue>
#include <iostream>
#include "Sched.h"

class Semaphore {
private:
    char resource_name[64];
    int sema_value;
    std::queue<TCB*> sema_queue;

public:
    Semaphore(const char* name);
    void down(TCB* task);
    void up();
    void dump(int level);
};

#endif