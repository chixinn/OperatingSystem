# shell说明文档

## 文档大纲

![截屏2020-03-31下午5.13.17.png](https://i.loli.net/2020/03/31/vIAF2XklUzRsiwy.png)



## 编译方法

在目录下输入`clang -o shell shell.c tool.c command.c mytop.c`即可编译。

输入`./shell`即可运行。

## 支持的功能

### 内置

Cd/mytop(top)/exit/histoy n

### program

管道/重定向/带参数运行的程序。

[^1]:这里管道暂只支持单个管道和重定向符号，不支持管道和重定向混用。

## 支持的样例

1. cd/your/path

2. ls(-a/-l -a)
3. ls -a -l > outfile
4. vi file
5. grep "parameters" < inputfile
6. Ls -a -l | grep a
7. vi file &
8. mytop
9. history n
10. Exit

## 各文件说明

### shell.c

shell函数的主体框架：

````C
int main()
{
   /*主体循环*/
    while(1)
    {
        printf("$");
        /*fgets从文件标准输入一次读一行，
        当键入文件结束符（ctrl+D),fgets返回一个null指针*/
        fgets(buf,MAXLINE,stdin);
        /*把末尾的回车去掉*/
        char *find;
        find=strchr(buf,'\n');
        if(find)
            *find='\0';
      /*保存历史命令*/
        char *tmp = (char*)malloc(sizeof(buf));
        strcpy(tmp,buf);
        history[historycount]=tmp ;
        historycount++;
      /*解析命令*/
        struct cmdline l=parse_line(buf);
      /*执行命令*/
        execmd(l,history,historycount);

    }
    exit(0);

}

````

### shell.h

包含函数声明/常数定义/编译所用的头文件

````C
#define MAXLINE 1024 /*命令最大长度*/
#define STD_INPUT 0/*标准输入*/
#define STD_OUTPUT 1/*标准输出*/
#define MAX_ARGC 5/*程序命令programA argv1 argv2参数最大大小*/
````

命令行结构：

````C
struct cmdline
{
    char *buf;/*键盘键入的原始字符串*/
    char *parameters[MAXLINE];/*按空格分割的参数args*/
    int argc;/*参数个数*/
    int is_background;/*是否是后置命令*/
    int is_special;/*是否有特殊符号，如>,<,>>,|*/
    int special_position;/*特殊符号位置*/
    int cmdnumber;/*程序个数*/

};
````

函数声明：

````C 
int issamestring(const char *a,const char *b);/*辅助函数，判断字符串是否相等*/
struct cmdline parse_line(char buf[]);/*parse*/
void execmd(struct cmdline l,char *history[],int historycount);

/*built-in cmds*/
void exe_mytop();
void exe_cd(char *parameters[],char* history[],int historycount);
void exe_history(char *parameters[],char* history[],int historycount);

/*支持的program*/
void exe_ls(struct cmdline oneline);
void  exe_grep(struct cmdline oneline);
void exe_vi(struct cmdline oneline);

/*redirection and pipe*/
void redirect(struct cmdline oneline);
void pipeline(struct cmdline oneline);
void pipel(char *process1[],char *process2[]);
````

### tool.c

包含了解析命令行、判断是否相等字符串、判断是否内置命令、执行命令的函数。

### command.c

Shell.h中声明的函数的执行。

#### program命令

##### ls/grep

ls/grep两个命令均是对文件IO逻辑的考察，逻辑相同，下以ls为例进行说明。

````C
void exe_ls(struct cmdline oneline){
  /*如果没有特殊符号*/
    if(oneline.is_special==0){
        int pid,status;
        pid=fork();
        if(pid<0)
            printf("fork error!");
        else if(pid ==0){
            execvp("ls",oneline.parameters);
        }
        else{
                waitpid(-1,&status,0);
        }
    }
    else{
      /*如果有特殊符号，判断特殊符号的类别*/
        if ((issamestring(">", 	oneline.parameters[oneline.special_position]) == 1)||(issamestring("<", oneline.parameters[oneline.special_position]) == 1)||(issamestring(">>", oneline.parameters[oneline.special_position]) == 1))
          /*特殊符号为重定向时*/
            redirect(oneline);
        else{  
           /*特殊符号为管道时*/
            pipeline(oneline);
        }
    }
````



##### vi and background job

linux的vi file &很奇怪，是通过将子进程的标准输入输出映射到/dev/null实现的。

我自己在minix3环境下实验了一下，发现无论是我自己的shell还是minix3原来的shell，都有如下提示：

`Vi's standard input and output must be a terminal`

````C
void exe_vi(struct cmdline oneline)
{

    int pid,status;
    pid=fork();
    if(pid<0){
        printf("fork error!");
        exit(1);
    }
    if(pid ==0) {
        if (oneline.is_background == 1) {
            /*子进程的标准输入、输出映射成/Dev/null*/
            close(0);//关闭标准输入
            open("/dev/null", O_RDONLY);//标准输入0指向空文件
            close(1);//关闭标准输出
            open("/dev/null", O_RDONLY);//标准输入1指向空文件
            signal(SIGCHLD, SIG_IGN);
            execvp("vi", oneline.parameters);
        } else {
            execvp("vi", oneline.parameters);
        }
    }
    else{
        waitpid(-1,&status,0);
    }

}
````

其余的background文件的实施 需要看父进程是否wait子进程。

##### redirection

````C
/*管道和重定向*/
void redirect(struct cmdline oneline) {
		/*提取特殊符号前的数组argv1,argv2*/
    char *argv1[MAX_ARGC];
    char *argv2[MAX_ARGC];
    for(int i=0;i<MAX_ARGC;i++){
        argv1[i]=NULL;
        argv2[i]=NULL;
    }
    for (int i = 0; i < oneline.special_position; i++) {
        argv1[i] = oneline.parameters[i];
    }
  /*注意execvp和NULL */
    argv1[oneline.special_position] = NULL;

    int j=oneline.special_position+1;
    for (int i = 0; i < oneline.argc - oneline.special_position-1; i++) {
        argv2[i] = oneline.parameters[j];
        j++;
    }
    argv2[oneline.argc-oneline.special_position] = NULL;//execvp使用；提取特殊命令后的programA 和program B//


    int pid, status, fd;


    if ((issamestring(">", oneline.parameters[oneline.special_position]) == 1)||(issamestring(">>", oneline.parameters[oneline.special_position]) == 1)){
        fd = dup(1);//复制标准输出,此时dup返回的是最新空闲,即让一个最新空闲fd也指向1
        close(1);//关闭标准输出
     }
    else {
        fd = dup(0);//复制，此时最新空闲的是fd
        close(0);//关闭标准输入
}
     /*先看文件是否存在，存在就覆盖写，不存在就创建写入*/
        /*fd要设置成全局变量，便于父进程对于文件描述符的恢复*/
    pid = fork();
    if (pid < 0) {
            printf("fork error!");
            exit(1);
    }
    else if (pid == 0) {
      /*子进程*/
        if(issamestring(">", oneline.parameters[oneline.special_position]) == 1){
            open(argv2[0], O_WRONLY | O_CREAT | O_TRUNC);//返回的fd是1
        }
        else if(issamestring(">>", oneline.parameters[oneline.special_position]) == 1){
            open(argv2[0],  O_WRONLY | O_CREAT | O_APPEND);//返回的fd是1
        }
        else
        {
            open(argv2[0], O_RDONLY);
        }
        execvp(argv1[0],argv1);
    }
    else{
      /*父进程*/
        waitpid(-1, &status, 0);
        if ((issamestring(">", oneline.parameters[oneline.special_position]) == 1)||(issamestring(">>", oneline.parameters[oneline.special_position]) == 1))
            close(1);
        else
            close(0);
        dup(fd);//将当前最小的文件描述符1，指向fd指向的；
        close(fd);//不让fd再指向1，即恢复最开始的状态
    }
    /*这里注意文件标识符的恢复问题*/
    /*文件表示符的恢复应该放在父进程里"执行*/
 }
````

##### pipe

pipe的主体框架

````C
	/*提取Program A /B,此处同redirection*/				
				char *argv1[MAX_ARGC];
        char *argv2[MAX_ARGC];
        for(int i=0;i<MAX_ARGC;i++){
            argv1[i]=NULL;
            argv2[i]=NULL;
        }
        for (int i = 0; i < oneline.special_position; i++) {
            argv1[i] = oneline.parameters[i];
        }
        argv1[oneline.special_position] = NULL;
        int j=oneline.special_position+1;
        for (int i = 0; i < oneline.argc - oneline.special_position-1; i++){
            argv2[i] = oneline.parameters[j];
            j++;
        }
        argv2[oneline.argc-oneline.special_position] = NULL;


		/*fork出两个子进程分别执行programA/programB */
        int status;
        if(fork()==0){
            pipel(argv1,argv2);
            exit(0);
        }
        else{
            waitpid(-1,&status,0);
        }
````



#### mytop命令

mytop框架如下，参照minix3的top.c框架

输出内容：

	1. 总体内存大小，空闲内存大小，缓存大小；
 	2. 总体CPU使用占比。

````C
void exe_mytop(){
    print_memifo();
    getkinfo();
    print_cpu();
}
````

下面对mytop.c的函数做具体解释，部分宏定义来源于top.c参考源码。

````C
/*在psinfo文件下解析进程*/
struct proc {
    int p_flags;
    endpoint_t p_endpoint;
    pid_t p_pid;
    u64_t p_cpucycles[CPUTIMENAMES];
    int p_priority;
    endpoint_t p_blocked;
    time_t p_user_time;
    vir_bytes p_memory;
    uid_t p_effuid;
    int p_nice;
};
struct tp {
    struct proc *p;
    u64_t ticks;
};
/*注意ticks的动态变化*/
struct proc *proc = NULL, *prev_proc = NULL;

/*mytop的主体函数*/
void print_memifo(void);
void getkinfo(void);
void print_cpu(void);

/* print_cpu组成函数
1.getprocs->parsedir->parsefile;
2.printprocs->cputicks的计算*/
void get_procs(void);
void print_procs(struct proc *prev_proc,struct proc *proc,int cputimemode);
void parse_dir(void);
void parse_file(pid_t pid);
/*print_procs*/
u64_t cputicks(struct proc *p1, struct proc *p2, int timemode);
````





