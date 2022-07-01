#ifndef _PROC_H_
#define _PROC_H_

#include "riscv.h"

typedef struct trapframe {
    // space to store context (all common registers)
    /* offset:0   */ riscv_regs regs;

    // process's "user kernel" stack
    /* offset:248 */ uint64 kernel_sp;
    // pointer to smode_trap_handler
    /* offset:256 */ uint64 kernel_trap;
    // saved user process counter
    /* offset:264 */ uint64 epc;

    //kernel page table
    /* offset:272 */ uint64 kernel_satp;
} trapframe;

// PKE kernel supports at most 32 processes
#define NPROC 32

// possible status of a process
enum proc_status {
    FREE,            // unused state
    READY,           // ready state
    RUNNING,         // currently running
    BLOCKED,         // waiting for something
    ZOMBIE,          // terminated but not reclaimed yet
};

// types of a segment
enum segment_type {
    CODE_SEGMENT,    // ELF segment
    DATA_SEGMENT,    // ELF segment
    STACK_SEGMENT,   // runtime segment
    CONTEXT_SEGMENT, // trapframe segment
    SYSTEM_SEGMENT,  // system segment
};

// the VM regions mapped to a user process
typedef struct mapped_region {
    uint64 va;       // mapped virtual address
    uint32 npages;   // mapping_info is unused if npages == 0
    uint32 seg_type; // segment type, one of the segment_types
} mapped_region;

// the extremely simple definition of process, used for begining labs of PKE
typedef struct process {
    // pointing to the stack used in trap handling.
    uint64 kstack;
    // user page table
    pagetable_t pagetable;
    // trapframe storing the context of a (User mode) process.
    trapframe *trapframe;

    // points to a page that contains mapped_regions
    mapped_region *mapped_info;
    // next free mapped region in mapped_info
    int total_mapped_region;

    // process id
    uint64 pid;
    // process status
    int status;
    // parent process
    struct process *parent;
    // next queue element
    struct process *queue_next;

    // accounting
    int tick_count;
} process;

//TODO(lab3_2_challenge):（1）process.h里添加信号量结构体
/*
 * value表示信号量的值，
 * occupied表示当前信号量是否已被使用，在分配信号量时选取的信号量数组中occupied属性为0的信号量。
 * wait_head和wait_tail分别为当前正在等待该信号量的进程队列的队列头指针和队列尾指针。
 */
typedef struct {
    int value, occupied;
    process *wait_head, *wait_tail;
} semaphore;
int alloc_sem(int);
int inc_sem(int);
int dec_sem(int);

// switch to run user app
void switch_to(process *);

// initialize process pool (the procs[] array)
void init_proc_pool();

// allocate an empty process, init its vm space. returns its pid
process *alloc_process();

// reclaim a process, destruct its vm space and free physical pages.
int free_process(process *proc);

// fork a child from parent
int do_fork(process *parent);

// current running process
extern process *current;
// virtual address of our simple heap
extern uint64 g_ufree_page;

#endif
