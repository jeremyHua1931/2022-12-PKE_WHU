/*
 * Below is the given application for lab1_1. 
 * 
 * You can build this app (as well as our PKE OS kernel) by command:
 * $ make
 *
 * Or run this app (with the support from PKE OS kernel) by command:
 * $ make run 
 */

#include "user_lib.h"

int main(void) {
  printu("Hello world!\n");

  exit(0);
}

// TODO: 解决lab1-1 打印Helloword
/*
(一)运行命令
     spike ./obj/riscv-pke ./obj/app_helloworld
==>结果
-----------------------------------
In m_start, hartid:0
HTIF is available!
(Emulated) memory size: 2048 MB
Enter supervisor mode...
Application: ./obj/app_helloworld
Fail on openning the input application program.

System is shutting down with exit code -1.
-----------------------------------
    (1)打印Hello world!失败,所以去找打印函数 printu() ->跳转 user_lib.c

 */
