#include "Sched.h"

Scheduler::Scheduler() {
    head = nullptr;
    current = nullptr;
    next_id = 1;
}

int Scheduler::create_task(const char* name) {
    TCB* newTask = new TCB;
    newTask->task_id = next_id++;
    strcpy(newTask->task_name, name);
    newTask->state = READY;
    newTask->next = nullptr;

    if (!head) {
        head = newTask;
        current = head;
    } else {
        TCB* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newTask;
    }

    return newTask->task_id;
}

void Scheduler::yield() {
    if (!current) return;

    current->state = READY;
    current = current->next ? current->next : head;

    while (current->state != READY) {
        current = current->next ? current->next : head;
    }

    current->state = RUNNING;
}

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

void Scheduler::garbage_collect() {
    TCB* temp = head;
    TCB* prev = nullptr;

    while (temp) {
        if (temp->state == DEAD) {
            if (prev) prev->next = temp->next;
            else head = temp->next;

            delete temp;
            temp = (prev) ? prev->next : head;
        } else {
            prev = temp;
            temp = temp->next;
        }
    }
}

void Scheduler::dump(int level) {
    std::cout << "\n--- Process Table ---\n";
    TCB* temp = head;

    while (temp) {
        std::cout << "Task: " << temp->task_name
                  << " | ID: " << temp->task_id
                  << " | State: ";

        switch (temp->state) {
            case RUNNING: std::cout << "RUNNING"; break;
            case READY: std::cout << "READY"; break;
            case BLOCKED: std::cout << "BLOCKED"; break;
            case DEAD: std::cout << "DEAD"; break;
        }

        std::cout << std::endl;
        temp = temp->next;
    }
}

TCB* Scheduler::get_current() {
    return current;
}