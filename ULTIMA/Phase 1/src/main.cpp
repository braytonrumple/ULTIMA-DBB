#include "Sched.h"
#include "Sema.h"
#include <iostream>

int main() {
    Scheduler sched;
    Semaphore sem("Printer");

    // =========================================================
    // TEST CASE 1: Scheduler Initialization + Task Creation
    // Purpose: Verify tasks are created and scheduler starts properly.
    // =========================================================
    std::cout << "\n=== TEST CASE 1 ===\n";

    int t1 = sched.create_task("Task1");
    int t2 = sched.create_task("Task2");
    int t3 = sched.create_task("Task3");

    sched.get_current()->state = RUNNING;
    sched.dump(1);

    // =========================================================
    // TEST CASE 2: Semaphore Locking (down)
    // Purpose: Show resource is acquired by one task and others block.
    // =========================================================
    std::cout << "\n=== TEST CASE 2 ===\n";

    sem.down(sched.get_current());
    sched.yield();

    sem.down(sched.get_current());
    sched.yield();

    sem.down(sched.get_current());

    sched.dump(1);
    sem.dump(1);

    // =========================================================
    // TEST CASE 3: Semaphore Release (up)
    // Purpose: Show blocked tasks are released correctly.
    // =========================================================
    std::cout << "\n=== TEST CASE 3 ===\n";

    sem.up();
    sched.yield();

    sched.dump(1);
    sem.dump(1);

    // =========================================================
    // TEST CASE 4: Scheduler Stability
    // Purpose: Ensure scheduler still runs after semaphore use.
    // =========================================================
    std::cout << "\n=== TEST CASE 4 ===\n";

    sched.yield();
    sched.yield();

    sched.dump(1);

    return 0;
}