#include "ipc.h"
#include <iostream>
#include <cstring>
#include <queue>
#include "Sched.h"

// constructor
IPC::IPC(Scheduler* s) {
    sched = s; // connect IPC to scheduler so we can access tasks
}

// =========================
// SEND MESSAGE
// =========================
int IPC::Message_Send(int s_id, int d_id, const char* text, int type) {

    TCB* dest = sched->get_task_by_id(d_id);
    TCB* src  = sched->get_task_by_id(s_id);

    if (!dest || !src) return -1;

    Message msg;

    msg.source_id = s_id;                 // who sent it
    msg.dest_id = d_id;                   // who receives it
    msg.arrival_time = time(nullptr);     // timestamp of message

    msg.type.type_id = type;              // message type id
    strcpy(msg.type.description, "Text"); // simple label

    msg.size = strlen(text);              // message size

    // copy message text safely
    strncpy(msg.text, text, sizeof(msg.text));
    msg.text[sizeof(msg.text) - 1] = '\0';

    // push into destination mailbox queue
    dest->mailbox.push(msg);

    return 0;
}

// =========================
// RECEIVE MESSAGE
// =========================
int IPC::Message_Receive(int task_id, Message &msg) {

    TCB* task = sched->get_task_by_id(task_id);
    if (!task) return -1;

    if (task->mailbox.empty()) {
        return 0; // no message available
    }

    msg = task->mailbox.front(); // get first message
    task->mailbox.pop();         // remove it

    return 1;
}

// =========================
// COUNT MESSAGES (PER TASK)
// =========================
int IPC::Message_Count(int task_id) {

    TCB* task = sched->get_task_by_id(task_id);
    if (!task) return -1;

    return task->mailbox.size();
}

// =========================
// COUNT MESSAGES (SYSTEM)
// =========================
int IPC::Message_Count() {

    int total = 0;

    for (int i = 1; i < 100; i++) {
        TCB* task = sched->get_task_by_id(i);
        if (task) {
            total += task->mailbox.size();
        }
    }

    return total;
}

// =========================
// PRINT MAILBOX (FIXED FORMAT - REQUIRED BY RUBRIC)
// =========================
void IPC::Message_Print(int task_id) {

    TCB* task = sched->get_task_by_id(task_id);
    if (!task) return;

    std::queue<Message> copy = task->mailbox;

    // REQUIRED FORMAT START
    std::cout << "\nTask Number: " << task_id << "\n";
    std::cout << "Message Count: " << copy.size() << "\n";
    std::cout << "Mail Box:\n";

    // EXACT HEADER FORMAT EXPECTED
    std::cout << "Source Task-id | Destination Task-id | Message Content | Message Size | Message Type | Message Arrival Time\n";

    // print all messages in mailbox
    while (!copy.empty()) {

        Message m = copy.front();
        copy.pop();

        std::cout << m.source_id << " | "
                  << m.dest_id << " | "
                  << m.text << " | "
                  << m.size << " | "
                  << m.type.type_id << " | "
                  << m.arrival_time
                  << "\n";
    }
}

// =========================
// DELETE ALL MESSAGES
// =========================
int IPC::Message_DeleteAll(int task_id) {

    TCB* task = sched->get_task_by_id(task_id);
    if (!task) return -1;

    while (!task->mailbox.empty()) {
        task->mailbox.pop();
    }

    return 0;
}

// =========================
// IPC SYSTEM DUMP
// =========================
void IPC::ipc_Message_Dump() {

    std::cout << "\n========== IPC SYSTEM DUMP ==========\n";

    for (int i = 1; i < 100; i++) {

        TCB* task = sched->get_task_by_id(i);

        if (task) {
            std::cout << "Task " << i
                      << " | Messages: " << task->mailbox.size() << "\n";
        }
    }

    std::cout << "=====================================\n";
}