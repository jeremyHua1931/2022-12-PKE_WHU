/*
 * Machine-mode startup codes
 */

#include "util/types.h"
#include "kernel/riscv.h"
#include "kernel/config.h"
#include "spike_interface/spike_utils.h"

//
// global variables are placed in the .data section.
// stack0 is the privilege mode stack(s) of the proxy kernel on CPU(s)
// allocates 4KB stack space for each processor (hart)
//
__attribute__((aligned(16))) char stack0[4096 * NCPU];

// sstart is the supervisor state entry point
extern void s_start();  // defined in kernel/kernel.c

// htif is defined in kernel/machine/spike_htif.c, marks the availability of HTIF
extern uint64 htif;
// g_mem_size is defined in kernel/machine/spike_memory.c, size of the emulated memory
extern uint64 g_mem_size;

//
// get the information of HTIF (calling interface) and the emulated memory by
// parsing the Device Tree Blog (DTB, actually DTS) stored in memory.
//
// the role of DTB is similar to that of Device Address Resolution Table (DART)
// in Intel series CPUs. it records the details of devices and memory of the
// platform simulated using Spike.
//
void init_dtb(uint64 dtb) {
  // defined in kernel/machine/htif.c, enabling Host-Target InterFace (HTIF)
  query_htif(dtb);
  if (htif) sprint("HTIF is available!\r\n");

  // defined in kernel/machine/fdt.c, obtain information about emulated memory
  query_mem(dtb);
  sprint("(Emulated) memory size: %ld MB\n", g_mem_size >> 20);
}

//
// delegate (almost all) interrupts and most exceptions to S-mode.
// after delegation, syscalls will handled by the PKE OS kernel running in S-mode.
//
static void delegate_traps() {
  if (!supports_extension('S')) {
    // confirm that our processor supports supervisor mode. abort if not.
    sprint("S mode is not supported.\n");
    return;
  }

  uintptr_t interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
  uintptr_t exceptions = (1U << CAUSE_MISALIGNED_FETCH) | (1U << CAUSE_FETCH_PAGE_FAULT) |
                         (1U << CAUSE_BREAKPOINT) | (1U << CAUSE_LOAD_PAGE_FAULT) |
                         (1U << CAUSE_STORE_PAGE_FAULT) | (1U << CAUSE_USER_ECALL);

  write_csr(mideleg, interrupts);
  write_csr(medeleg, exceptions);
  assert(read_csr(mideleg) == interrupts);
  assert(read_csr(medeleg) == exceptions);
}

//
// m_start: machine mode C entry point.
//
void m_start(uintptr_t hartid, uintptr_t dtb) {
  // init the spike file interface (stdin,stdout,stderr)
  spike_file_init();
  sprint("In m_start, hartid:%d\n", hartid);

  // init HTIF (Host-Target InterFace) and memory by using the Device Table Blob (DTB)
  init_dtb(dtb);

  // set previous privilege mode to S (Supervisor), and will enter S mode after 'mret'
  write_csr(mstatus, ((read_csr(mstatus) & ~MSTATUS_MPP_MASK) | MSTATUS_MPP_S));

  // set M Exception Program Counter to sstart, for mret (requires gcc -mcmodel=medany)
  write_csr(mepc, (uint64)s_start);

  // delegate all interrupts and exceptions to supervisor mode.
  delegate_traps();

  // switch to supervisor mode and jump to s_start(), i.e., set pc to mepc
  asm volatile("mret");
}
