#ifndef SEMA_H
#define SEMA_H

#include <queue>
#include <iostream>

// Forward declaration - no need to include Sched.h
struct TCB;

class Semaphore {
private:
    char resource_name[64];
    int sema_value;
    std::queue<TCB*> sema_queue;  // TCB* only needs forward declaration

public:
    Semaphore(const char* name);
    void down();
    void down(TCB* task);
    void up();
    void dump(int level);
};

#endif