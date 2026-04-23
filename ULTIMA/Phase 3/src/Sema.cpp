#include "Sema.h"
#include "Sched.h"
#include <cstring>

Semaphore::Semaphore(const char* name) {
    strncpy(resource_name, name, 63);
    resource_name[63] = '\0';
    sema_value = 1;
}

void Semaphore::down() {
    if (sema_value == 1) {
        sema_value = 0;
    }
}

void Semaphore::down(TCB* task) {
    if (sema_value == 1) {
        sema_value = 0;
    } else {
        task->state = BLOCKED;
        sema_queue.push(task);
    }
}


void Semaphore::up() {
    if (!sema_queue.empty()) {
        TCB* task = sema_queue.front();
        sema_queue.pop();
        task->state = READY;
    } else {
        sema_value = 1;
    }
}

void Semaphore::dump(int level) {
    std::cout << "\n--- Semaphore Dump ---\n";
    std::cout << "Resource: " << resource_name << std::endl;
    std::cout << "Value: " << sema_value << std::endl;

    std::cout << "Queue: ";
    std::queue<TCB*> temp = sema_queue;

    while (!temp.empty() && temp.front() != NULL) {
        std::cout << temp.front()->task_id << " -> ";
        temp.pop();
    }
    std::cout << "NULL\n";
}