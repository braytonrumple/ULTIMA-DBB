#include <iostream>
#include "Sched.h"
#include "ipc.h"

int main() {

    std::cout << "===== IPC SYSTEM START =====\n\n";

    Scheduler sched;
    IPC ipc(&sched);

    // =========================
    // STEP 1: CREATE TASKS
    // =========================
    int t1 = sched.create_task("Task 1");
    int t2 = sched.create_task("Task 2");
    int t3 = sched.create_task("Task 3");

    std::cout << "Tasks created: " << t1 << ", " << t2 << ", " << t3 << "\n\n";

    // =========================
    // STEP 2: INITIAL SCHEDULER STATE
    // =========================
    std::cout << "INITIAL SCHEDULER STATE:\n";
    sched.dump(0);
    std::cout << "\n";

    // =========================
    // STEP 3: SEND MESSAGES
    // =========================
    std::cout << "SENDING MESSAGES...\n";

    ipc.Message_Send(t1, t3, "Hello from Task 1 to Task 3", 1);
    ipc.Message_Send(t2, t3, "Hello from Task 2 to Task 3", 1);
    ipc.Message_Send(t3, t1, "Reply from Task 3 to Task 1", 1);
    ipc.Message_Send(t1, t2, "Update from Task 1 to Task 2", 1);

    std::cout << "Messages sent.\n\n";

    // =========================
    // STEP 4: IPC INTERNAL STATE DUMP
    // =========================
    std::cout << "IPC INTERNAL STATE DUMP:\n";
    ipc.ipc_Message_Dump();
    std::cout << "\n";

    // =========================
    // STEP 5: MAILBOX STATE PER TASK
    // =========================
    std::cout << "MAILBOX STATE PER TASK:\n";

    ipc.Message_Print(t1);
    ipc.Message_Print(t2);
    ipc.Message_Print(t3);

    std::cout << "\n";

    // =========================
    // STEP 6: MESSAGE COUNTS
    // =========================
    std::cout << "MESSAGE COUNTS:\n";
    std::cout << "Task 1: " << ipc.Message_Count(t1) << "\n";
    std::cout << "Task 2: " << ipc.Message_Count(t2) << "\n";
    std::cout << "Task 3: " << ipc.Message_Count(t3) << "\n\n";

    // =========================
    // STEP 7: RECEIVE MESSAGES
    // =========================
    std::cout << "RECEIVING MESSAGES (DETAILED):\n";

    Message msg;

    while (ipc.Message_Receive(t3, msg)) {
        std::cout << "[T3] From " << msg.source_id
                  << " -> " << msg.dest_id
                  << " | Text: " << msg.text
                  << " | Size: " << msg.size << "\n";
    }

    while (ipc.Message_Receive(t1, msg)) {
        std::cout << "[T1] From " << msg.source_id
                  << " -> " << msg.dest_id
                  << " | Text: " << msg.text
                  << " | Size: " << msg.size << "\n";
    }

    while (ipc.Message_Receive(t2, msg)) {
        std::cout << "[T2] From " << msg.source_id
                  << " -> " << msg.dest_id
                  << " | Text: " << msg.text
                  << " | Size: " << msg.size << "\n";
    }

    std::cout << "\n";

    // =========================
    // STEP 8: FINAL STATE
    // =========================
    std::cout << "FINAL SCHEDULER STATE:\n";
    sched.dump(0);

    std::cout << "\nFINAL IPC STATE:\n";
    ipc.ipc_Message_Dump();

    std::cout << "\n===== IPC SYSTEM END =====\n";

    return 0;
}