#include <iostream>
#include "Sched.h"
#include "ipc.h"
#include "mmu.h"

void Mem_Print_All(mmu &memory);

int main() {

    std::cout << "- Memory Tests -\n";

    mmu memory(1024, '.', 64);
    Scheduler sched(&memory);
    IPC ipc(&sched);

    std::cout << "\n- Test 1: Tasks Creation-\n";
    int t1 = sched.create_task("Task1");
    int t2 = sched.create_task("Task2");
    int t3 = sched.create_task("Task3");

    sched.dump(0);
    memory.Print(&sched);
    memory.Print_MemInfo();
    Mem_Print_All(memory);

    std::cout << "\n- Test 2: Memory Write -\n";

    TCB* task = sched.get_task_by_id(t1);
    memory.Mem_Write(task->mem_handle, 'A');
    memory.Mem_Write(task->mem_handle, 'B');
    memory.Mem_Write(task->mem_handle, 'C');

    memory.Print_MemInfo();
    memory.Mem_Dump(0, 128);

    std::cout << "\n- Test 3: Memory Read -\n";

    char ch;
    memory.Mem_Read(task->mem_handle, &ch);
    std::cout << "Read char: " << ch << std::endl;

    memory.Print_MemInfo();

    std::cout << "\n- Test 4: Messing Passing -\n";

    ipc.Message_Send(t1, t2, "Hello", 1);
    ipc.Message_Send(t2, t1, "Reply", 1);

    ipc.Message_Print(t1);
    ipc.Message_Print(t2);

    std::cout << "\n-Test 5: Memory Freeing -\n";

    memory.Mem_Free(task->mem_handle);

    memory.Print(&sched);
    memory.Print_MemInfo();
    Mem_Print_All(memory);

    std::cout << "\n- Test 6: Coalesce -\n";

    memory.Mem_Coalesce();

    memory.Print(&sched);
    memory.Mem_Dump(0, 256);
    Mem_Print_All(memory);

    std::cout << "\n- Test 7: failure-\n";

    int fail = memory.Mem_Alloc(5000); // too big

    if(fail == -1){
        std::cout << "Allocation failed\n";
    }

    std::cout << "\n-Final States-\n";

    sched.dump(0);
    ipc.ipc_Message_Dump();
    memory.Print(&sched);
    memory.Print_MemInfo();
    Mem_Print_All(memory);

    return 0;
}