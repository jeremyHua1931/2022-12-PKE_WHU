/*
 * Below is the given application for lab1_2.
 * This app attempts to issue M-mode instruction in U-mode, and consequently raises an exception.
 */

#include "user_lib.h"
#include "util/types.h"

int main(void) {
  printu("Going to hack the system by running privilege instructions.\n");
  // we are now in U(user)-mode, but the "csrw" instruction requires M-mode privilege.
  // Attempting to execute such instruction will raise illegal instruction exception.
  asm volatile("csrw sscratch, 0");
  exit(0);
}

///*
// * 修改前  spike ./obj/riscv-pke ./obj/app_illegal_instruction
// * ------------------------------------------------------------------
//In m_start, hartid:0
//HTIF is available!
//(Emulated) memory size: 2048 MB
//Enter supervisor mode...
//Application: ./obj/app_illegal_instruction
//Application program entry point (virtual address): 0x0000000081000000
//Switch to user mode...
//Going to hack the system by running privilege instructions.
//call handle_illegal_instruction to accomplish illegal instruction interception for lab1_2.
//
//System is shutting down with exit code -1.
//---------------------------------------------------------------------
// *(1)原因
// * 应用程序“企图”执行不能在用户模式 (U模式) 运行的特权级指令：csrw sscratch, 0
// * ==>返回异常提示
// * 查找RISC-V体系结构的相关文档，这类异常属于非法指令异常，即CAUSE_ILLEGAL_INSTRUCTION，它对应的异常码是02 (见kernel/riscv.h中的定义)
// *
// * PKE操作系统内核在启动时会将部分异常和中断“代理”给S模式处理，但是它是否将CAUSE_ILLEGAL_INSTRUCTION这类异常也进行了代理呢？
//先要去查看是否内核将哪些异常和中断代理给了S模式
// * 这就要研究m_start()函数在执行delegate_traps()函数时设置的代理规则了，先查看delegate_traps()函数的代码，在kernel/machine/minit.c文件中找到它对应的代码
// *
// *
// *
// * /
