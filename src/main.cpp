#include "Sched.h"
#include "Sema.h"

int main() {
    Scheduler sched;
    Semaphore sem("Printer");

    int t1 = sched.create_task("Task1");
    int t2 = sched.create_task("Task2");
    int t3 = sched.create_task("Task3");

    sched.get_current()->state = RUNNING;

    // Simulate tasks trying to access resource
    sem.down(sched.get_current());  // Task1 gets it
    sched.yield();

    sem.down(sched.get_current());  // Task2 blocked
    sched.yield();

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