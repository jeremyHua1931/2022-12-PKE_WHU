/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"
#include "pmm.h"
#include "vmm.h"
#include "spike_interface/spike_utils.h"

//
// implement the SYS_user_print syscall
//调用了 user_va_to_pa()
//该函数最终在第26行通过调用sprint将结果输出，
// 但是在输出前，需要将buf地址转换为物理地址传递给sprint，
// 这一转换是通过user_va_to_pa()函数完成的。而user_va_to_pa()函数的定义在kernel/vmm.c文件中定义
//
ssize_t sys_user_print(const char *buf, size_t n) {
    // buf is now an address in user space of the given app's user stack,
    // so we have to transfer it into phisical address (kernel is running in direct mapping).
    assert(current);
    char *pa = (char *) user_va_to_pa((pagetable_t) (current->pagetable), (void *) buf);
    sprint(pa);
    return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
    sprint("User exit with code:%d.\n", code);
    // in lab1, PKE considers only one app (one process).
    // therefore, shutdown the system when the app calls exit()
    shutdown(code);
}

//
// maybe, the simplest implementation of malloc in the world ... added @lab2_2
//
uint64 sys_user_allocate_page() {
    //分配了一个首地址为pa的物理页面
    void *pa = alloc_page();
    //pa对应的逻辑地址va = g_ufree_page
    uint64 va = g_ufree_page;
    // 46行对g_ufree_page进行了递增操作
    g_ufree_page += PGSIZE;
    //将pa映射给了va地址,==>g_ifree_page  kernel/process.c

    //vmm.c  user_vm_map()
    user_vm_map((pagetable_t) current->pagetable, va, PGSIZE, (uint64)
    pa,prot_to_type(PROT_WRITE | PROT_READ, 1));

    return va;
}

//
// reclaim a page, indicated by "va". added @lab2_2
//
uint64 sys_user_free_page(uint64 va) {
    user_vm_unmap((pagetable_t) current->pagetable, va, PGSIZE, 1);
    return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
    switch (a0) {
        case SYS_user_print:
            return sys_user_print((const char *) a1, a2);
        case SYS_user_exit:
            return sys_user_exit(a1);
            // added @lab2_2
        case SYS_user_allocate_page:
            return sys_user_allocate_page();
        case SYS_user_free_page:
            return sys_user_free_page(a1);
        default:
            panic("Unknown syscall %ld \n", a0);
    }
}
