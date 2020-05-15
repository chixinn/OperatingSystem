/*添加chrt函数的实现*/
/*在实现中通过_syscall(调用号)向系统服务传递*/
#include <sys/cdefs.h>
#include "namespace.h"
#include <lib.h>

#include <string.h>
#include <unistd.h>

#ifdef __weak_alias
__weak_alias(fork, _chrt)
#endif
/*建立一条消息,将deadline的数据包装在消息内*/
/*利用_syscall函数去调用pm的主控函数*/
/*alarm实现超时强制终止*/
int chrt(long deadline)
{
  struct timespec time;
  message m;//消息结构体
  memset(&m, 0, sizeof(m));
  alarm((unsigned int)deadline);//设置alarm;
  if(deadline<0)
  return 0;
  if(deadline>0){
    clock_gettime(CLOCK_REALTIME,&time);
    deadline =time.tv_sec+deadline;//nowtime+deadline;
  }
  /*记录进程终止时间赋值给deadline*/
  m.m2_l1=deadline;

  return(_syscall(PM_PROC_NR, PM_CHRT, &m));//在消息结构体中将deadline放入
  //PM_PROC_NR:进程号,调用的服务,消息结构体;
}
