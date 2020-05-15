#include <sys/cdefs.h>
#include "namespace.h"
#include <lib.h>
#include<stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifdef __weak_alias
__weak_alias(fork, _chrt)
#endif

int chrt(long deadline)
{
  struct timespec time;
  message m;
  memset(&m, 0, sizeof(m));
  alarm((unsigned int)deadline);
  if(deadline<0)
  	return 0;
  if(deadline>0){
    clock_gettime(CLOCK_REALTIME,&time);
    deadline =time.tv_sec+deadline;//nowtime+deadline;
  }
  m.m2_l1=deadline;

  return(_syscall(PM_PROC_NR, PM_CHRT, &m));//在消息结构体中将deadline放入
}
