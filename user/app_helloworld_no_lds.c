/*
 * Below is the given application for lab2_1.
 * This app runs in its own address space, in contrast with in direct mapping.
 */

#include "user_lib.h"
#include "util/types.h"

int main(void) {
  printu("Hello world!\n");
  exit(0);
}

//修改前  spike ./obj/riscv-pke ./obj/app_helloworld_no_lds
//In m_start, hartid:0
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
//Application: ./obj/app_helloworld_no_lds
//        Application program entry point (virtual address): 0x00000000000100f6
//Switch to user mode...
//You have to implement user_va_to_pa (convert user va to pa) to print messages in lab2_1.
//
//System is shutting down with exit code -1.

/*
 * 结果: 未能打印出helloworld
 * (1)通过指导书分析
 * lab2_1的应用app_helloworld_no_lds（实际上就是lab1_1中的app_helloworld，
 * 不同的地方在于没有用到lab1_1中的user/user.lds来约束逻辑地址）只包含一个代码段，
 * 它的起始地址为0x0000000000010000（即0x10000）
 * ==>对比前面讨论的物理内存布局，spike模拟的RISC-V机器并无处于0x10000的物理地址空间与其对应。这样，我们就需要通过Sv39虚地址管理方案
 *    将0x10000开始的代码段，映射到app_helloworld_no_lds中代码段实际被加载到的物理内存
 *    ==>lab2中的应用加载是通过kernel/kernel.c 中的load_user_program函数实现的
 * (2) printu打印失败
 * printu是一个典型的系统调用（参考lab1_1的内容），它的执行逻辑是通过ecall指令，陷入到内核（S模式）完成到屏幕的输出。
 * 然而，对于内核而言，显然不能继续使用“Hello world!\n”的逻辑地址对它进行访问，而必须将其转换成物理地址
 *
 * 与实验1 同理 跳转到 kernel/syscall.c sys_user_print()
 *
 */

