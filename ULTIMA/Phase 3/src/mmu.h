#ifndef ULTIMA_DBB_MMU_H
#define ULTIMA_DBB_MMU_H

#include "Sema.h"
#include <iostream>
#include <vector>

class Scheduler;

struct Block {
    int start;
    int size;
    bool free;
    int mem_handle;
    Block* next;
};

struct MemInfo{
    int base;
    int limit;
    int offset;
    int read_offset;
    int write_offset;
    int request_size;
    bool active;
};

class mmu{
    private:
        unsigned char* memory;
        int size;
        int page_size;
        int next_handle=0;

        Block* head;

        Semaphore memory_semaphore;

        std::vector<MemInfo> mem_info;

    public:
        mmu(int size, char default_value, int page_size);
        ~mmu();

        void Mem_Dump(int starting_from, int num_bytes);

        void Print(Scheduler* sched);

        int Mem_Alloc(int size);
        int Mem_Free(int memory_handle);

        int Mem_Read(int memory_handle, char *ch);
        int Mem_Read(int memory_handle, int offset_from_beg, int text_size, char *text);
        int Mem_Write(int memory_handle, char ch);
        int Mem_Write(int memory_handle, int offset_from_beg, int text_size, char *text);

        int Mem_Left();
        int Mem_Largest();
        int Mem_Smallest();
        int Mem_Coalesce();

        void Mem_Print_All(mmu &memory);
        void Print_Memory_Table(mmu &memory);
        void Print_MemInfo();


};
#endif //ULTIMA_DBB_MMU_H