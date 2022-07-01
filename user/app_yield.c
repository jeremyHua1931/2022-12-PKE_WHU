/*
 * The application of lab3_2.
 * parent and child processes intermittently give up their processors.
 */

#include "user/user_lib.h"
#include "util/types.h"

int main(void) {
  uint64 pid = fork();
  uint64 rounds = 0xffff;
  if (pid == 0) {
    printu("Child: Hello world! \n");
    for (uint64 i = 0; i < rounds; ++i) {
      if (i % 10000 == 0) {
        printu("Child running %ld \n", i);
        yield();
      }
    }
  } else {
    printu("Parent: Hello world! \n");
    for (uint64 i = 0; i < rounds; ++i) {
      if (i % 10000 == 0) {
        printu("Parent running %ld \n", i);
        yield();
      }
    }
  }

  exit(0);
  return 0;
}

/*
 * 和lab3_1一样，以上的应用程序通过fork系统调用创建了一个子进程，接下来，父进程和子进程都进入了一个很长的循环。
 * 在循环中，无论是父进程还是子进程，在循环的次数是10000的整数倍时，除了打印信息外都调用了yield()函数，来释放自己的执行权（即CPU）。
 */

//spike obj/riscv-pke obj/app_yield
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
//Application: obj/app_yield
//        CODE_SEGMENT added at mapped info offset:3
//Application program entry point (virtual address): 0x000000000001017c
//going to insert process 0 to ready queue.
//going to schedule process 0 to run.
//User call fork.
//will fork a child from parent 0.
//in alloc_proc. user frame 0x0000000087faf000, user stack 0x000000007ffff000, user kstack 0x0000000087fae000
//going to insert process 1 to ready queue.
//Parent: Hello world!
//Parent running 0
//You need to implement the yield syscall in lab3_2.

/*
 * (1)分析
 * ：完善yield系统调用，实现进程执行过程中的主动释放CPU的动作 ==>kernel/syscall.c
 *
 */