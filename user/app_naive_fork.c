/*
 * The application of lab3_1.
 * it simply forks a child process.
 */

#include "user/user_lib.h"
#include "util/types.h"

int main(void) {
    //主进程调用fork()函数，后者产生一个系统调用，基于主进程这个模板创建它的子进程。
  uint64 pid = fork();
  if (pid == 0) {
    printu("Child: Hello world!\n");
  } else {
    printu("Parent: Hello world! child id %ld\n", pid);
  }

  exit(0);
}

//spike obj/riscv-pke obj/app_naive_fork
//        In m_start, hartid:0
//HTIF is available!
//(Emulated) memory size: 2048 MB
//        Enter supervisor mode...
//PKE kernel start 0x0000000080000000, PKE kernel end: 0x0000000080009000, PKE kernel size: 0x0000000000009000 .
//free physical memory address: [0x0000000080009000, 0x0000000087ffffff]
//kernel memory manager is initializing ...
//        KERN_BASE 0x0000000080000000
//physical address of _etext is: 0x0000000080005000
//kernel page table is on
//        Switch to user mode...
//        in alloc_proc. user frame 0x0000000087fbc000, user stack 0x000000007ffff000, user kstack 0x0000000087fbb000
//User application is loading.
//Application: obj/app_naive_fork
//        CODE_SEGMENT added at mapped info offset:3
//Application program entry point (virtual address): 0x0000000000010078
//going to insert process 0 to ready queue.
//going to schedule process 0 to run.
//User call fork.
//will fork a child from parent 0.
//in alloc_proc. user frame 0x0000000087faf000, user stack 0x000000007ffff000, user kstack 0x0000000087fae000
//You need to implement the code segment mapping of child in lab3_1.
//
//System is shutting down with exit code -1.

/*
 * (1)分析
 * ，应用程序的fork动作并未将子进程给创建出来并投入运行。按照提示，我们需要在PKE操作系统内核中实现子进程到父进程代码段的映射，以最终完成fork动作。
 *
 * (2)系统调用
 * user/app_naive_fork.c --> user/user_lib.c --> kernel/strap_vector.S --> kernel/strap.c --> kernel/syscall.c
 */