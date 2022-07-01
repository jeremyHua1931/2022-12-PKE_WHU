/*
 * The application of lab2_3.
 */

#include "user_lib.h"
#include "util/types.h"

//
// compute the summation of an arithmetic sequence. for a given "n", compute
// result = n + (n-1) + (n-2) + ... + 0
// sum_sequence() calls itself recursively till 0. The recursive call, however,
// may consume more memory (from stack) than a physical 4KB page, leading to a page fault.
// PKE kernel needs to improved to handle such page fault by expanding the stack.
//
uint64 sum_sequence(uint64 n) {
  if (n == 0)
    return 0;
  else
    return sum_sequence( n-1 ) + n;
}

int main(void) {
  // we need a large enough "n" to trigger pagefaults in the user stack
  uint64 n = 1000;

  printu("Summation of an arithmetic sequence from 0 to %ld is: %ld \n", n, sum_sequence(1000) );
  exit(0);
}
/*
 * 递归求解等差数列
 * ==>函数调用的路径会被完整地保存在栈（stack）中，也就是说函数的下一次调用会将上次一调用的现场（包括参数）压栈，直到n=0时依次返回到最开始给定的n值，
 * 从而得到最终的计算结果。显然，在以上计算等差数列的和的程序中，n值给得越大，就会导致越深的栈，而栈越深需要的内存空间也就越多
 *
 * 应用程序最开始被载入（并装配为用户进程）时，它的用户态栈空间（栈底在0x7ffff000，即USER_STACK_TOP）仅有1个4KB的页面。显然，只要以上的程序给出的n值“足够”大，
 *
 * 就一定会“压爆”用户态栈。而以上运行结果中，出问题的地方（即handle_page_fault后出现的地址，0x7fffdff8）也恰恰在用户态栈所对应的空间。
 * 以上分析表明，之所以运行./obj/app_sum_sequence会出现错误（handle_page_fault），
 * 是因为给sum_sequence()函数的n值太大，把用户态栈“压爆”了。
 *
 * 类似同 lab1-2 异常处理
 * 本实验中，我们处理的是缺页异常（app_sum_sequence.c执行的显然是“合法”操作），不能也不应该将应用进程杀死。正确的做法是：首先，通过异常的类型，
 * 判断我们处理的确实是缺页异常；接下来，判断发生缺页的是不是用户栈空间，如果是则分配一个物理页空间，最后将该空间通过vm_map“粘”到用户栈上以扩充用户栈空间。
 */
