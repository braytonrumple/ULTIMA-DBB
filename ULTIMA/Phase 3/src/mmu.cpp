#include "mmu.h"
#include "sema.h"
#include "sched.h"
#include <string>

using namespace std;



mmu::mmu(int size, char default_value, int page_size)
    :memory_semaphore("MMU_LOCK"){
    this->size = size;
    this->page_size = page_size;
    memory = new unsigned char[size];

    for(int i = 0; i < size; i++){
        memory[i] = default_value;
    }

    head = new Block();
    head->start = 0;
    head->size = size;
    head->free=true;
    head->mem_handle = -1;
    head->next = NULL;
}
mmu::~mmu(){
    delete[] memory;

    Block* current = head;
    while(current != NULL){
        Block* temp = current; //set temp to current
        current = current->next;//move current to next
        delete temp;
    }
}

int mmu::Mem_Alloc(int size){

    memory_semaphore.down();
    int block_size = (size + page_size - 1)/page_size;
    int allocation_size = block_size*page_size;

    Block* current = head;

    while (current != NULL){
        if(current->free && current->size >= allocation_size){
            int handle = next_handle++;
            if(handle >= mem_info.size()){
                mem_info.resize(handle + 1);
            }
            mem_info[handle].base = current->start;
            mem_info[handle].limit = allocation_size;
            mem_info[handle].offset = 0;
            mem_info[handle].read_offset = 0;
            mem_info[handle].write_offset = 0;
            mem_info[handle].request_size = size;
            mem_info[handle].active = true;

            //USE SEMA FOR THIS
            //for exact fit
            if(current->size == allocation_size){
                current->free = false;
                current->mem_handle = handle;

                memory_semaphore.up();
                return handle;
            }

            //use SEMA FOR THIS
            //needs to be split
            Block* split = new Block();
            split->start = current->start;
            split->size = allocation_size;
            split->free = false;
            split->mem_handle = handle;

            Block* remainer = new Block();
            remainer->start = current->start + allocation_size;
            remainer->size = current->size - allocation_size;
            remainer->free = true;
            remainer->mem_handle = -1;

            split->next = remainer;
            remainer->next = current->next;


            //curr block replacement
            if(current == head){
                head = split;
            }else{
                Block* previous = head;
                while(previous->next != current){
                    previous = previous->next;
                }
                previous->next = split;
            }
            delete current;

            memory_semaphore.up();
            return handle;

        }
        current = current->next;
    }
    memory_semaphore.up();
    return -1;//for no space
}

int mmu::Mem_Free(int memory_handle){
    memory_semaphore.down();
    Block* current = head;

    while(current != NULL){
        if(current->mem_handle == memory_handle){
            break;//when it founds it
        }
        current = current->next;
    }

    if(current == NULL){
        memory_semaphore.up();
        return -1;
    }

    current->free = true;
    current->mem_handle = -1;

    for(int i = 0; i < current->size; i++){
        memory[current->start + i] = '#';
    }

    if(memory_handle >=0 && memory_handle < mem_info.size()){
        mem_info[memory_handle].base = -1;
        mem_info[memory_handle].limit = 0;
        mem_info[memory_handle].offset = 0;
        mem_info[memory_handle].read_offset = 0;
        mem_info[memory_handle].write_offset = 0;
        mem_info[memory_handle].active = false;
    }



    Block* curr = head;
    while(curr != NULL && curr->next != NULL){
        if(curr->free && curr->next->free){
            Block* next = curr->next;
            curr->size += next->size;
            curr->next = next->next;

            delete next;
            continue;

        }
        curr = curr->next;
    }
    memory_semaphore.up();
    return 0;
}

int mmu::Mem_Read(int memory_handle, char *ch){
    memory_semaphore.down();

    //check if exsists
    if(memory_handle >= mem_info.size() || !mem_info[memory_handle].active){
        memory_semaphore.up();
        return -1;
    }

    MemInfo &location = mem_info[memory_handle];


    if(location.read_offset >= location.request_size){
        memory_semaphore.up();
        return -1;
    }

    int physical_location = location.read_offset + location.base;
    *ch = memory[physical_location];

    location.read_offset++;

    memory_semaphore.up();
    return 0;
}

int mmu::Mem_Write(int memory_handle, char ch){
    memory_semaphore.down();

    //check if exsists
    if(memory_handle < 0 || memory_handle >= mem_info.size() || !mem_info[memory_handle].active){
        memory_semaphore.up();
        return -1;
    }

    MemInfo &location = mem_info[memory_handle];

    if(location.write_offset >= location.request_size){
        memory_semaphore.up();
        return -1;
    }

    int physical_location = location.write_offset + location.base;

    memory[physical_location] = ch;

    location.write_offset++;

    memory_semaphore.up();
    return 0;

}

int mmu::Mem_Read(int memory_handle, int offset_from_beg, int text_size, char *text){
    memory_semaphore.down();

    //check if exsists
    if(memory_handle >= mem_info.size() || !mem_info[memory_handle].active){
        memory_semaphore.up();
        return -1;
    }

    MemInfo &location = mem_info[memory_handle];

    if(offset_from_beg < 0 ||offset_from_beg + text_size > location.request_size){
        memory_semaphore.up();
        return -1;
    }

    int start = location.base + offset_from_beg;

    for(int i = 0; i < text_size; i++){
        text[i] = memory[start + i];
    }
    memory_semaphore.up();
    return 0;
}

int mmu::Mem_Write(int memory_handle, int offset_from_beg, int text_size, char *text){
    memory_semaphore.down();

    //check if exsists
    if(memory_handle < 0 ||memory_handle >= mem_info.size() || !mem_info[memory_handle].active){
        memory_semaphore.up();
        return -1;
    }

    MemInfo &location = mem_info[memory_handle];

    if(offset_from_beg < 0 || offset_from_beg + text_size > location.request_size){
        memory_semaphore.up();
        return -1;
    }

    int start = location.base + offset_from_beg;

    for(int i = 0; i < text_size; i++){
        memory[start + i] = text[i];
    }

    memory_semaphore.up();
    return 0;
}

void mmu::Mem_Dump(int starting_from, int num_bytes){
    for(int i = starting_from; i < starting_from + num_bytes && i < size; i++){
        char c = memory[i];

        if(std::isprint(c)){
            std::cout<< c;
        }else{
            std::cout << c;}
    }
    std::cout << std::endl;
}

int mmu::Mem_Left(){
    int total = 0;
    Block* current = head;
    while(current != NULL){
        if(current->free){
            total += current->size;
        }
        current = current->next;
    }
    return total;
}

int mmu::Mem_Largest(){
    int largest = 0;
    Block* current = head;
    while(current != NULL){
        if(current->free && current->size > largest){
            largest = current->size;
        }
        current = current->next;
    }
    return largest;
}

int mmu::Mem_Smallest(){
    int smallest = -1;
    Block* current = head;
    while(current != NULL){
        if(current->free){
            if(smallest == -1 || current->size < smallest){
                smallest = current->size;
            }
        }

        current = current->next;
    }
    return smallest;
}
int mmu::Mem_Coalesce(){
    int merge = 0;
    Block* current = head;

    while(current != NULL && current->next != NULL){
        if(current->free && current->next->free){
            Block* next = current->next;

            current->size += next->size;
            current->next = next->next;

            delete next;
            merge++;

            continue;
        }
        current = current->next;
    }

    current = head;
    while(current != NULL){
        if(current->free){
            for(int i = 0; i < current->size; i++){
                memory[current->start + i] = '.';
            }
        }
        current = current->next;
    }
    return merge;
}

void Mem_Print_All(mmu &memory){
    std::cout << "Mem_Left: " << memory.Mem_Left() << std::endl;
    std::cout << "Mem_Largest: " << memory.Mem_Largest() << std::endl;
    std::cout << "Mem_Smallest: " << memory.Mem_Smallest() << std::endl;
}


void mmu::Print(Scheduler* sched){
    std::cout << "\n MEMORY TABLE \n";
    std::cout << "Status | Handle | Start | End | Size | Current Location | Task-ID\n";

    Block* current = head;

    while(current != NULL) {

        std::string current_loc= "NA";
        int task_id = -1;


        if(!current->free && sched != NULL && current->mem_handle>=0 && current->mem_handle < mem_info.size()
            && mem_info[current->mem_handle].active){

            int loc = mem_info[current->mem_handle].base + mem_info[current->mem_handle].offset;
            current_loc = std::to_string(loc);

            TCB* task = sched->get_task_by_handle(current->mem_handle);
            if(task != NULL){
                task_id = task->task_id;


            }
        }
        std::cout << (current->free ? "Free  " : "Used  ")
                  << " | "
                  << current->mem_handle
                  << " | "
                  << current->start
                  << " | "
                  << (current->start + current->size - 1)
                  << " | "
                  << current->size
                  << " | "
                  << current_loc
                  << " | "
                  << task_id
                  << "\n";

        current = current->next;
    }
}
void mmu::Print_MemInfo(){
    std::cout << "\n MEMORY USAGE TABLE \n";
    std::cout << "Handle | Base | Limit | Offset | ReadPtr | WritePtr | Active\n";

    for(int i = 0; i < mem_info.size(); i++){
        if(mem_info[i].active){
            std::cout << i << " | "
                      << mem_info[i].base << " | "
                      << mem_info[i].limit << " | "
                      << mem_info[i].offset << " | "
                      << mem_info[i].read_offset << " | "
                      << mem_info[i].write_offset << " | "
                      << mem_info[i].active << "\n";
        }
    }
}
