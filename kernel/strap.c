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
      panic( "You need to implement the operations that actually handle the page fault in lab2_3.\n" );

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
    // we need to handle the timer trap @lab1_3.
    if (cause == CAUSE_USER_ECALL) {
        handle_syscall(current->trapframe);
    } else if (cause == CAUSE_MTIMER_S_TRAP) {  //soft trap generated by timer interrupt in M mode
        /*
         * 该函数首先读取scause寄存器的内容，如果内容等于CAUSE_MTIMER_S_TRAP的话，
         * 说明是M态传递上来的时钟中断动作，就调用handle_mtimer_trap()函数进行处理
         */
        handle_mtimer_trap();
    } else {
        sprint("smode_trap_handler(): unexpected scause %p\n", read_csr(scause));
        sprint("            sepc=%p stval=%p\n", read_csr(sepc), read_csr(stval));
        panic("unexpected exception happened.\n");
    }

    // continue (come back to) the execution of current process.
    switch_to(current);
}
