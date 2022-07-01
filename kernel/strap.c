/*
 * Utility functions for trap handling in Supervisor mode.
 */

#include "riscv.h"
#include "process.h"
#include "strap.h"
#include "syscall.h"
#include "pmm.h"
#include "vmm.h"
#include "util/functions.h"

#include "spike_interface/spike_utils.h"

//
// handling the syscalls. will call do_syscall() defined in kernel/syscall.c
//
static void handle_syscall(trapframe *tf) {
    // tf->epc points to the address that our computer will jump to after the trap handling.
    // for a syscall, we should return to the NEXT instruction after its handling.
    // in RV64G, each instruction occupies exactly 32 bits (i.e., 4 Bytes)
    tf->epc += 4;

    // TODO (lab1_1): remove the panic call below, and call do_syscall (defined in
    // kernel/syscall.c) to conduct real operations of the kernel side for a syscall.
    // IMPORTANT: return value should be returned to user app, or else, you will encounter
    // problems in later experiments!
//  panic( "call do_syscall to accomplish the syscall and lab1_1 here.\n" );
/*
 * 调用 do_syscall() kernel/syscall.c
 * long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7);
 */

    //regs.a1存的是字符串的首地址
    long returnCode = do_syscall(tf->regs.a0, tf->regs.a1, tf->regs.a2, tf->regs.a3, tf->regs.a4, tf->regs.a5,
                                 tf->regs.a6, tf->regs.a7);
//    long returnCode=do_syscall(tf->regs.a0,tf->regs.a1,0,0,0,0,0,0);
    //a0保存(函数调用时) 的参数/函数的返回值
    tf->regs.a0 = returnCode;

    //重新运行 make clean;make
    //spike ./obj/riscv-pke ./obj/app_helloworld
    /*
     *In m_start, hartid:0
HTIF is available!
(Emulated) memory size: 2048 MB
Enter supervisor mode...
Application: ./obj/app_helloworld
Application program entry point (virtual address): 0x0000000081000000
Switch to user mode...
Hello world!=============>成功!
User exit with code:0.
System is shutting down with exit code 0.
     */
}

//
// global variable that store the recorded "ticks". added @lab1_3
static uint64 g_ticks = 0;
//
// added @lab1_3
//
void handle_mtimer_trap() {
    sprint("Ticks %d\n", g_ticks);
    // TODO (lab1_3): increase g_ticks to record this "tick", and then clear the "SIP"
    // field in sip register.
    // hint: use write_csr to disable the SIP_SSIP bit in sip.
//    panic("lab1_3: increase g_ticks by one, and clear SIP field in sip register.\n");
    //确保我们的系统持续正常运行，该计数应每次都会完成加一操作
    g_ticks++;
    /*
     * 由于处理完中断后，SIP (Supervisor Interrupt Pending，即S模式的中断等待寄存器) 寄存器中的SIP_SSIP位仍然为1 (由M态的中断处理函数设置)，
     * 如果该位持续为1的话会导致我们的模拟RISC-V机器始终处于中断状态。
     * 所以，handle_mtimer_trap()还需要对SIP的SIP_SSIP位清零，以保证下次再发生时钟中断时，M态的函数将该位置一会导致S模式的下一次中断
     * ==kernel/machine/mtrap.c  handle_timer()  write_csr(sip, SIP_SSIP);
     */
    write_csr(sip, 0);
    //再次运行,成功

}

//
// the page fault handler. added @lab2_3. parameters:
// sepc: the pc when fault happens;
// stval: the virtual address that causes pagefault when being accessed.
//
void handle_user_page_fault(uint64 mcause, uint64 sepc, uint64 stval) {
  sprint("handle_page_fault: %lx\n", stval);
  switch (mcause) {
    case CAUSE_STORE_PAGE_FAULT:
      // TODO (lab2_3): implement the operations that solve the page fault to
      // dynamically increase application stack.
      // hint: first allocate a new physical page, and then, maps the new page to the
      // virtual address that causes the page fault.
//      panic( "You need to implement the operations that actually handle the page fault in lab2_3.\n" );
/*
 * ===>变量定义错误
 * switch的几个case语句在同一个作用域（因为case语句只是标签，它们共属于一个swtich语句块），所以如果在某个case下面声明变量的话，]
 * 对象的作用域是在俩个花括号之间 也就是整个switch语句，其他的case语句也能看到，这样的话就可能导致错误。
 * 解决方案：
 * 我们可以通过在case后面的语句加上大括号处理，之所以加大括号就是为了明确我们声明的变量的作用域，就是仅仅在本case之中，其实为了更规范的写switch-case语句，我们应该在case语句后边加大括号。在case语句后加一个分号也可以解决问题！
 */
/*
 * 实现缺页处理的思路如下：通过输入的参数stval（存放的是发生缺页异常时，程序想要访问的逻辑地址）判断缺页的逻辑地址在用户进程逻辑地址空间中的位置，
 * 看是不是比USER_STACK_TOP小，
 * 且比我们预设的可能的用户栈的最小栈底指针要大（这里，我们可以给用户栈空间一个上限，例如20个4KB的页面），
 * 若满足，则为合法的逻辑地址（本例中不必实现此判断，默认逻辑地址合法）。分配一个物理页，将所分配的物理页面映射到stval所对应的虚拟地址上
 */
      {
          void* pa = alloc_page();
          //要求中默认逻辑地址合法
          //prot_to_type(PROT_WRITE | PROT_READ, 1)
          //U (User) 位表示该页是不是一个用户模式页。如果U=1，表示用户模式下的代码可以访问该页，否则就表示不能访问。
          //R (Read) 、W (Write) 和X (eXecutable) 位分别表示此页对应的实页是否可读、可写和可执行。
          //&0xfffffffffffff000 去掉12位页偏移
          user_vm_map(current->pagetable, stval & 0xfffffffffffff000, PGSIZE, (uint64)(pa), prot_to_type(PROT_WRITE | PROT_READ, 1));
      }
//      {
//          //仅在缺页时运行----判断 stval与 user_stack_bottom的关系
//          //当触发缺页异常的地址刚好在原本用户态栈底旁边时，则进行扩展栈空间的处理，其余地址的访存则是非法的缺页异常，应直接终止进程的运行。
//          //因为一次只会存取一个 int ,所以最多只会超出 32 ,机器位长64, 一个数据最多占64 位， 所以差值不应该大于64， 过多就是非法访问地址
//          if (user_stack_bottom - stval < 64 && user_stack_bottom - stval>0) {
//              //同 lab2_1
//              void *pa = alloc_page();
//              user_vm_map(current->pagetable, stval & 0xfffffffffffff000, PGSIZE, (uint64)(pa),
//                          prot_to_type(PROT_WRITE | PROT_READ, 1));
//              user_stack_bottom -= PGSIZE;
//
//          } else {
//              panic("this address is not available!");
//          }
//      }
       break;
    default:
      sprint("unknown page fault.\n");
      break;
  }
}

//
// kernel/smode_trap.S will pass control to smode_trap_handler, when a trap happens
// in S-mode.
//
void smode_trap_handler(void) {
    // make sure we are in User mode before entering the trap handling.
    // we will consider other previous case in lab1_3 (interrupt).
    if ((read_csr(sstatus) & SSTATUS_SPP) != 0) panic("usertrap: not from user mode");

    assert(current);
    // save user process counter.
    current->trapframe->epc = read_csr(sepc);

    // if the cause of trap is syscall from user application.
    // read_csr() and CAUSE_USER_ECALL are macros defined in kernel/riscv.h
    uint64 cause = read_csr(scause);

  // use switch-case instead of if-else, as there are many cases since lab2_3.
  switch (cause) {
    case CAUSE_USER_ECALL:
      handle_syscall(current->trapframe);
      break;
    case CAUSE_MTIMER_S_TRAP:
      handle_mtimer_trap();
      break;
    case CAUSE_STORE_PAGE_FAULT:
    case CAUSE_LOAD_PAGE_FAULT:
      // the address of missing page is stored in stval
      // call handle_user_page_fault to process page faults
      handle_user_page_fault(cause, read_csr(sepc), read_csr(stval));
      break;
    default:
      sprint("smode_trap_handler(): unexpected scause %p\n", read_csr(scause));
      sprint("            sepc=%p stval=%p\n", read_csr(sepc), read_csr(stval));
      panic( "unexpected exception happened.\n" );
      break;
  }

  // continue (come back to) the execution of current process.
  switch_to(current);
}
