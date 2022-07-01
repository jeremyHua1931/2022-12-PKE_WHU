/*
 * The application of lab3_3.
 * parent and child processes never give up their processor during execution.
 */

#include "user/user_lib.h"
#include "util/types.h"

int main(void) {
    /*
     * 和lab3_2类似，lab3_3给出的应用仍然是父子两个进程，他们的执行体都是两个大循环。
     * 但与lab3_2不同的是，这两个进程在执行各自循环体时，都没有主动释放CPU的动作。
     * 显然，这样的设计会导致某个进程长期占据CPU，而另一个进程无法得到执行
     */
  uint64 pid = fork();
  uint64 rounds = 100000000;
  uint64 interval = 10000000;
  uint64 a = 0;
  if (pid == 0) {
    printu("Child: Hello world! \n");
    for (uint64 i = 0; i < rounds; ++i) {
      if (i % interval == 0) printu("Child running %ld \n", i);
    }
  } else {
    printu("Parent: Hello world! \n");
    for (uint64 i = 0; i < rounds; ++i) {
      if (i % interval == 0) printu("Parent running %ld \n", i);
    }
  }

  exit(0);
  return 0;
}

//spike obj/riscv-pke obj/app_two_long_loops
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
//Application: obj/app_two_long_loops
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
//Parent running 10000000
//Ticks 0
//You need to further implement the timer handling in lab3_3.
//
//System is shutting down with exit code -1.

/*
 * (1)分析
 * 于进程的执行体很长，执行过程中时钟中断被触发（输出中的“Ticks 0”）。
 * 显然，我们可以通过利用时钟中断来实现进程的循环轮转调度，避免由于一个进程的执行体过长，导致系统中其他进程无法得到调度的问题！
 *
 * (2)
 * 如果单纯为了实现进程的轮转，避免单个进程长期霸占CPU的情况，
 * 只需要简单地在时钟中断被触发时做重新调度即可。然而，为了实现时间片的概念，
 * 以及控制进程在单时间片内获得的执行长度，我们在kernel/sched.h文件中定义了“时间片”的长度：
 * #define TIME_SLICE_LEN  2
 *
 * ==>可以看到时间片的长度（TIME_SLICE_LEN）为2个ticks，这就意味着我们要每隔两个ticks触发一次进程重新调度动作。
 *
 * ==>为配合调度的实现，我们在进程结构中定义了整型成员（参见多任务环境下进程的封装）tick_count，完善kernel/strap.c文件中的rrsched()函数，以实现循环轮转调度时，
 *
 */