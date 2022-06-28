/*
 * Below is the given application for lab1_3.
 * This app performs a long loop, during which, timers are
 * generated and pop messages to our screen.
 */

#include "user_lib.h"
#include "util/types.h"

int main(void) {
  printu("Hello world!\n");
  int i;
  for (i = 0; i < 100000000; ++i) {
    if (i % 5000000 == 0) printu("wait %d\n", i);
  }

  exit(0);

  return 0;
}

//修改前  spike ./obj/riscv-pke ./obj/app_long_loop
//---------------------------------------------------------------
//In m_start, hartid:0
//HTIF is available!
//(Emulated) memory size: 2048 MB
//        wait 0
//wait 5000000
//wait 10000000
//machine trap(): unexpected mscause 0x8000000000000007
//mepc=0x00000000810000b0 mtval=0x0000000000000000
//unexpected exception happened in M-mode.
//
//System is shutting down with exit code -1.
//---------------------------------------------------------------

/*(1)原因
 * 从以上程序的运行结果来看，给定的程序并不是“不受干扰”地从开始运行到最终结束的，而是在运行过程中受到了系统的外部时钟中断 (timer irq) 的“干扰”！
 * 在这个实验中给出的PKE操作系统内核，在时钟中断部分并未完全做好，导致 (模拟) RISC-V机器碰到第一个时钟中断后就会出现崩溃。
 *
 * (2)时钟中断是在M模式下完成的 kernel/machine/minit.c  timerinit()
 */