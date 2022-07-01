/*
 * Below is the given application for lab2_2.
 */

#include "user_lib.h"
#include "util/types.h"

struct my_structure {
  char c;
  int n;
};

int main(void) {
    //分配空间存放 my_structure
    //先进行存储,然后进行打印

  struct my_structure* s = (struct my_structure*)naive_malloc();
  s->c = 'a';
  s->n = 1;

  printu("s: %lx, {%c %d}\n", s, s->c, s->n);

  naive_free(s);
  exit(0);
}

//运行  make run
//spike obj/riscv-pke obj/app_naive_malloc
//        In m_start, hartid:0
//HTIF is available!
//(Emulated) memory size: 2048 MB
//        Enter supervisor mode...
//PKE kernel start 0x0000000080000000, PKE kernel end: 0x0000000080007000, PKE kernel size: 0x0000000000007000 .
//free physical memory address: [0x0000000080007000, 0x0000000087ffffff]
//kernel memory manager is initializing ...
//        KERN_BASE 0x0000000080000000
//physical address of _etext is: 0x0000000080004000
//kernel page table is on
//        User application is loading.
//user frame 0x0000000087fbc000, user stack 0x000000007ffff000, user kstack 0x0000000087fbb000
//Application: obj/app_naive_malloc
//        Application program entry point (virtual address): 0x0000000000010078
//Switch to user mode...
//s: 0000000000400000, {a 1}
//You have to implement user_vm_unmap to free pages using naive_free in lab2_2.
//
//System is shutting down with exit code -1.
//make: *** [Makefile:120: run] Error 255

/*
 * (1)分析
 *  从输出结果来看，s: 0000000000400000, {a 1}的输出说明分配内存已经做好
 *  （也就是说naive_malloc函数及其内核功能的实现已完成），且打印出了我们预期的结果。
 *  但是，naive_free对应的功能并未完全做好
 *  ==>
 *  应用程序执行过程中的动态内存分配和回收，是操作系统中的堆（Heap）管理的内容。
 *  在本实验中，实际上是为PKE操作系统内核实现一个简单到不能再简单的“堆”。为实现naive_free()的内存回收过程
 *
 * (2)解决方法,先看内存如何分配,即 naive_malloc()函数如何实现的  kernel/syscall.c
 *
 */
