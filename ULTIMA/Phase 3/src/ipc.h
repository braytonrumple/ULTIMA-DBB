#ifndef IPC_H
#define IPC_H

#include <ctime>
#include <queue>
#include <cstring>
#include <iostream>

// =========================
// Message definitions
// =========================

struct Message_Type {
    int type_id;
    char description[64];
};

// Actual message structure - MOVE THIS UP
struct Message {
    int source_id;
    int dest_id;
    time_t arrival_time;
    Message_Type type;
    int size;
    char text[32];
};

// IMPORTANT: avoid circular dependency
class Scheduler;

// =========================
// IPC system
// =========================

class IPC {
private:
    Scheduler* sched;

public:
    IPC(Scheduler* s);

    int Message_Send(int s_id, int d_id, const char* text, int type);
    int Message_Receive(int task_id, Message &msg);  // Now Message is defined
    int Message_Count(int task_id);
    int Message_Count();
    void Message_Print(int task_id);
    int Message_DeleteAll(int task_id);
    void ipc_Message_Dump();
};

#endif