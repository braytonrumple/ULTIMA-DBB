#include "Sched.h"
#include "Sema.h"

int main() {
    Scheduler sched;
    Semaphore sem("Printer");

    int t1 = sched.create_task("Task1");
    int t2 = sched.create_task("Task2");
    int t3 = sched.create_task("Task3");

    sched.yield();

    // Simulate tasks trying to access resource
    std::cout << "Task " << sched.get_current()->task_id << " requesting resource\n";
    sem.down(sched.get_current());  // Task1 gets it
    sched.yield();

    std::cout << "Task " << sched.get_current()->task_id << " requesting resource\n";
    sem.down(sched.get_current());  // Task2 blocked
    sched.yield();

    std::cout << "Task " << sched.get_current()->task_id << " requesting resource\n";
    sem.down(sched.get_current());  // Task3 blocked

    sched.dump(1);
    sem.dump(1);

    // Release resource
    sem.up();
    sched.yield();

    sched.dump(1);
    sem.dump(1);

    return 0;
}