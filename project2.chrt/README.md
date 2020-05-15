# 进程调度

## 实现功能

1. chrt 系统调用:

   ```
   int chrt(long deadline)
   ```

   该系统调用实现的功能:

   1. deadline=0时:设置为普通进程;
   2. deadline>0时:设置为实时进程,即该进程应该在deadline秒内结束.如果到达进程结束时间点时该进程仍然没有结束,则超过时间强制停止该进程;
   3. deadline<0时:直接返回不成功;
   4. 设置内核进程表中中一个新增的条目,条目表示进程的实时属性;

2. EDF实时调度算法-当同时具有实时进程和普通进程的时候,优先执行实时进程:

   > deadline>0,deadline=0为普通进程,要么加一个普通和实时的区分,要么给所有进程都先赋值为deadline为0;

   1. 最早deadline的用户进程相对于其它用户进程具有更高的优先级,从而被优先调度运行;
   2. 可以在不同位置调用多次chrt,即在未强制结束前deadline可以改变;
   3. 未调用chrt则以普通用户进程在系统运行;
   4. 产生实时进程之后,该进程需要保证不影响该系统进程的优先级;
   5. 模拟实际调度的方式;(Option,优先级高概率高)

## 系统调用功能实现

进程调度由kernel完成,因此系统调用的任务是将deadline消息传入kernel.

对于参数小于0的chrt函数,系统调用不通知内核并直接返回1;

### 应用层

> 在应用层用_syscall将信息传递到服务层

1. /src/include/unistd.h中添加chrt函数定义(Y)

2. /src/minix/(包含操作系统使用的头文件)lib//libc//ssys/chrt.c中添加chrt函数的实现;

   ```c
   int chrt(long deadline)
   {
     message m;//消息结构体
     memset(&m, 0, sizeof(m));
     alarm((unsigned int)deadline);//设置alarm;
   
     /*记录进程终止时间赋值给deadline*/
     m.m2_l1=deadline;
   
     return(_syscall(PM_PROC_NR, PM_CHRT, &m));//在消息结构体中将deadline放入
     //PM_PROC_NR:进程号,调用的服务,消息结构体;
   }
   ```

3. /src/minix/lib/libc/sys中的Makefile.inc文件添加chrt.c条目;

### 服务层:

> 在服务层用_kernal_call将deadline信息传递到内核层;
>
> 向minix系统的进程管理服务器(src/servers/pm)中注册chrt

1. /src/minix/servers/pm/proto.h添加chrt.c函数定义

2. /src/minix/servers/pm/chrt.c添加chrt.c函数的声明

   ```c
   int do_chrt(){
     if(m_in.m2_l1<0)
      	return1;
      sys_chrt(who_p,m_in.m2_l1);
       //sys_chrt(who_p,消息结构体deadline消息)
       return OK;
   }
   ```

3. /src/minix/include/minix/callnr.h定义PM_CHRT编号

   注意PM_CHRT编号定义时,也同时修改系统调用总数;

4. /src/minix/servers/pm/Makefile中添加chrt.c条目

---

1. /src/minix/servers/pm/table.c中调用映射表;

2. /src/minix/include/minix/syslib.h中添加sys_chrt()定义

3. /src/minix/lib/libsys/sys_chrt.c中添加sys_chrt的实现;(系统调用的**主控函数**,负责从message中读取deadline值并通过_kernal_call传给内核)

   ```c
   #include"syslib.h"
   
   //声明风格模仿sys_fork.c
   int sys_chrt(who,deadline)
   long deadline;
   endpoint_t who;
   {
       message m;
       m.m2_l1=deadline;
       m.m2_l2=who;
       return _kernel_call(SYS_CHRT,&m);
   }
   ```

   

4. /minix/lib/libsys中的Makefile中添加sys_chrt.c条目

---

### 内核层

> 修改内核信息

1. 在/src/minix/kernel/system.h中添加do_chrt函数定义.

```c
int do_chrt(struct proc * caller, message *m_ptr);
#if ! USE_CHRT
#define do_chrt NULL
#endif
```

2. 在/src.minix/kernel/system/do_chrt中添加do_chrt函数实现(kernel层的主控函数)

   ```c
   int do_chrt(struct proc *caller, message *m_ptr){
       struct proc *rp;
       /*获得调用chrt的进程的proc结构*/
       rp=proc_addr(m_ptr->m2_i1);
       /*将消息结构体中的deadline赋值给该进程的p_deadline*/
       rp->p_deadline=m_ptr->m2_l1;
       return OK;
   }
   ```

3. /src/minix/kernel/system/ 中的Makefile.inc文件添加do_chrt.c条目

---

1. /src/minix/include/minix/com.h中定义SYS_CHRT编号

   ```c
   
   #  define SYS_CHRT (KERNEL_CALL + 58)
   /* Total */
   #define NR_SYS_CALLS	59	/* number of kernel calls */
   ```

2. /src/minix/kernel/system.c中添加SYS_CHRT编号到do_chrt的映射

   ````c
     map(SYS_CHRT, do_chrt);
   ````

3. /src/minix/commands/service/parse.c的system_tab中添加名称编号对.

4. /src/minix/kernel/config.h添加内核调用chrt的使用开关;

## 修改Minix进程调度算法

1. 在/minix/kernel下的proc.h中未每一个进程表项添加一项deadline,在内核层第二项函数实现中:rp->p_deadline中已经显示了p_deadline成员信息的添加;
2. 在调度函数中,将deadline>0的进程,为实时进程的优先级设置为 **用户进程**的最高优先级7;

---

### 原minix3进程调度

1. 多级调度算法:

   进程优先级数字7和7以上为用户进程,用户进程启动时比所有系统进程的优先级都要低;

2. 每个队列内毒采用时间片轮转调度算法:

   1. 当一个进程用完了它的时间片,则它被移到队列尾部并且分配一个新的时间片;

---

## Minix3的源码组织





i