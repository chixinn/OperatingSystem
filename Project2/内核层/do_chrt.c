#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <lib.h>
#include <minix/endpoint.h>
#include "kernel/system.h"
#include "kernel/vm.h"
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <minix/endpoint.h>
#include <minix/u64.h>
/*m2_i1: RA_PROC_NR the process number*/
/*m2_l1:RA_DEADLINE deadline*/

/*用消息结构体中的进程号,通过proc_addr定位内核中进程地址,然后将
消息结构体中的deadline赋值给该进程的p_deadline(这里已经在proc头文件中添加了该成员变量)*/

int do_chrt(struct proc *caller, message *m_ptr){
    struct proc *rp;
    /*获得调用chrt的进程的proc结构*/
    rp=proc_addr(m_ptr->m2_i1);
    /*#define proc_addr(n)      (&(proc[NR_TASKS + (n)]))*/
    /*将消息结构体中的deadline赋值给该进程的p_deadline*/
    rp->p_deadline=m_ptr->m2_l1;
    return OK;
}

