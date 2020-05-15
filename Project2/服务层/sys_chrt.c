#include"syslib.h"

//声明风格模仿sys_fork.c
int sys_chrt(who, deadline)
long deadline;
endpoint_t who;
{
    message m;
    m.m2_l1=deadline;
    m.m2_i1=who;
    return _kernel_call(SYS_CHRT,&m);
}