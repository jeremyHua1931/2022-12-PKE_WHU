#ifndef _PROC_H_
#define _PROC_H_

#include "riscv.h"

typedef struct trapframe {
    /*
     * 该结构除了记录进程上下文的RISC-V机器的通用寄存器组 (regs成员) 外，还包括很少
的其他成员 (如指向内核态栈顶的kernel_sp，指向内核态trap处理函数入口的
kernel_trap指针，进程执行的当前位置epc)
     */
  // space to store context (all common registers)
  /* offset:0   */ riscv_regs regs;

  // process's "user kernel" stack
  /* offset:248 */ uint64 kernel_sp;
  // pointer to smode_trap_handler
  /* offset:256 */ uint64 kernel_trap;
  // saved user process counter
  /* offset:264 */ uint64 epc;

  // kernel page table. added @lab2_1
  /* offset:272 */ uint64 kernel_satp;
}trapframe;

// the extremely simple definition of process, used for begining labs of PKE
typedef struct process {
  // pointing to the stack used in trap handling.
  uint64 kstack;
  // user page table
  pagetable_t pagetable;
  // trapframe storing the context of a (User mode) process.
  trapframe* trapframe;
}process;

// switch to run user app
void switch_to(process*);

// current running process
extern process* current;

#endif
