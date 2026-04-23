#include "Sched.h"
#include "ipc.h"
#include "Sema.h"
#include "mmu.h"

// constructor: setup scheduler
Scheduler::Scheduler() {
    head = nullptr;
    current = nullptr;
    next_id = 1;
    mem = nullptr;
}

Scheduler::Scheduler(mmu* memory){
    head = nullptr;
    current = nullptr;
    next_id = 1;
    mem = memory;
}

// create a new task and add to list
int Scheduler::create_task(const char* name) {
    TCB* newTask = new TCB;

    newTask->task_id = next_id++;     // give unique id
    strcpy(newTask->task_name, name); // copy name
    newTask->state = READY;           // start as ready
    newTask->next = nullptr;

    newTask->mailbox_semaphore = new Semaphore("mailbox");


    if(mem != nullptr) {
        newTask->mem_handle = mem->Mem_Alloc(64);

    }else{
        newTask->mem_handle = -1;
    }

    if(newTask->mem_handle == -1) {
        newTask->state = BLOCKED;
    }


    // if first task
    if (!head) {
        head = newTask;
        current = head;
    }
    else {
        // go to end of list
        TCB* temp = head;
        while (temp->next)
            temp = temp->next;

        temp->next = newTask;
    }

    return newTask->task_id;
}

// switch to next ready task
void Scheduler::yield() {
    if (!current) return;

    current->state = READY;

    // move to next or loop back
    current = current->next ? current->next : head;

    // skip non-ready tasks
    while (current->state != READY) {
        current = current->next ? current->next : head;
    }

    current->state = RUNNING;
}

// mark task as dead (not removed yet)
void Scheduler::kill_task(int id) {
    TCB* temp = head;

    while (temp) {
        if (temp->task_id == id) {
            temp->state = DEAD;
            return;
        }
        temp = temp->next;
    }
}

// remove dead tasks from memory
void Scheduler::garbage_collect() {
    TCB* temp = head;
    TCB* prev = nullptr;

    while (temp) {
        if (temp->state == DEAD) {
            if (prev)
                prev->next = temp->next;
            else
                head = temp->next;
            delete temp->mailbox_semaphore;
            delete temp;

            temp = (prev) ? prev->next : head;
        }
        else {
            prev = temp;
            temp = temp->next;
        }
    }
}

// print all tasks
void Scheduler::dump(int level) {
    std::cout << "\n--- Process Table ---\n";

    TCB* temp = head;

    while (temp) {
        std::cout << "Task: " << temp->task_name
                  << " | ID: " << temp->task_id
                  << " | State: ";

        if (temp->state == RUNNING) std::cout << "RUNNING";
        else if (temp->state == READY) std::cout << "READY";
        else if (temp->state == BLOCKED) std::cout << "BLOCKED";
        else std::cout << "DEAD";

        std::cout << std::endl;
        temp = temp->next;
    }
}

// return current running task
TCB* Scheduler::get_current() {
    return current;
}

// find task by id (used by IPC)
TCB* Scheduler::get_task_by_id(int id) {
    TCB* temp = head;

    while (temp) {
        if (temp->task_id == id)
            return temp;

        temp = temp->next;
    }

    return nullptr; // not found
}

TCB* Scheduler::get_task_by_handle(int mem_handle) {
    TCB* temp = head;

    while (temp) {
        if (temp->mem_handle == mem_handle)
            return temp;

        temp = temp->next;
    }

    return nullptr; // not found
}
