/*
 * The supporting library for applications.
 * Actually, supporting routines for applications are catalogued as the user 
 * library. we don't do that in PKE to make the relationship between application 
 * and user library more straightforward.
 */

#include "user_lib.h"
#include "util/types.h"
#include "util/snprintf.h"
#include "kernel/syscall.h"

//
int do_user_call(uint64 sysnum, uint64 a1, uint64 a2, uint64 a3, uint64 a4, uint64 a5, uint64 a6,
                 uint64 a7) {
  int ret;

  // before invoking the syscall, arguments of do_user_call are already loaded into the argument
  // registers (a0-a7) of our (emulated) risc-v machine.
  asm volatile(
      "ecall\n"
      "sw a0, %0"  // returns a 32-bit value
      : "=m"(ret)
      :
      : "memory");

  return ret;
  /* do_user_call执行 ecall 命令
   * ==>ecall指令用于向运行时环境发出请求，如系统调用 ( Trap (即我们通常理解的“系统调用”或者“软件中断”，))
   *    ecall指令的执行将根据a0寄存器中的值获得系统调用号，并使RISC-V转到S模式(因为pke操作系统内核启动时将所有的中断、异常、系统调用都代理给了S模式) 的trap处理入口执行
   *    (S模式--次高特权级的监管者模式),
   *    入口--(在kernel/strap_vector.S文件中定义) ：
   */

}

//
// printu() supports user/lab1_1_helloworld.c
//
int printu(const char* s, ...) {
  va_list vl;
  va_start(vl, s);

  char out[256];  // fixed buffer size.
  int res = vsnprintf(out, sizeof(out), s, vl);
  va_end(vl);
  const char* buf = out;
  size_t n = res < sizeof(out) ? res : sizeof(out);

  // make a syscall to implement the required functionality.
  //TODO: lab1-1 第三个思考题 字符串首地址存在了buf, buf即 a1中将存的值
  return do_user_call(SYS_user_print, (uint64)buf, n, 0, 0, 0, 0, 0);
  //==执行do_user_call, 跳转
}

//
// applications need to call exit to quit execution.
//
int exit(int code) {
  return do_user_call(SYS_user_exit, code, 0, 0, 0, 0, 0, 0); 
}
