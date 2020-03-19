#include <stdio.h>
#include "string.h"
#include "apue.h"//
#include <unistd.h>//magic number 0，1，2作为文件标识符的常量定义
#include<fcntl.h>//
#include<sys/stat.h>//
#include<sys/types.h>//



#define MAXLINE 1024
#define MAXLINES 64
#define MAXARGS 32
#define MAXHISTORYLINE 64
#define MASARGLEN 32

/*目前版本都未实现错误处理的wrapper和考虑C语言中字符串数组初始化的内存问题*/
/*未考虑输入的字符非已知命令或者是系统存在program的问题
 *未考虑所有调用时出现错误的边界问题*/
int main()
{
    int historycount = 0;
    char *cmdline[MAXLINE];
    char **parameters[MAXARGS][MASARGLEN];
    memset(parameters,0,sizeof(parameters));
    char **cmdhistory[MAXLINES][MAXLINE];
    memset(cmdhistory,0,sizeof(cmdhistory));


while(1)
{   
    /*打印prompt*/
    type_prompt();
    /*读取我当前输入的命令行数据*/
    cmdline = read_cmd();
    /*读取命令行数据并保存命令行数据历史*/
    cmdhistory[historycount] = cmdline; historycount++;
    /*解析命令行并将命令行分解成参数列表的形式*/
    parameters = parse_line(cmdline);
    /*根据命令名称分为二类分别处理*/
    execmd(parameters);

    /*Shell 主要是为用户提供了一个命令解释器，接收用户命令主要是调用相应的应用程序，
        实现Shell支持后台进程的运行。*/
   
    
}
return 0;
}
/*main函数的猪蹄框架设计思路为：
读命令、解析命令、调用命令，最后做相应的反应*/


/*各动作的具体实现*/

void type_prompt()
{
    printf("$");
}

/*读入行，使用getline函数*/
char *read_cmd(void)
{
    char *line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

/*解析命令行，默认所有参数只通过空格来区分  
  这里使用C库函数strtok
  strtok函数的原型为 char *strtok(char *s,char *delim)
  strtok函数的用法参考菜鸟教程
*/
char **parse_line(char *line[])
{
    int i = 0;
    char *str = line;
    char **parameters[MAXARGS][MASARGLEN];
    char *token = strtok(str, ' ');
    while(token != NULL)
    {
        parameters[i] = strtok(NULL, ' ');/*获取参数列表*/
        /*需要在参数解析的时候来判断有没有管道和重定向*/
        i++;
    }
    parameters[i] = NULL;
    return parameters;
}
/*根据命令名称分为二类分别处理*/
void  execmd(char **parameters[][])
{
    bool result = built_in(parameters);
    if(result == TRUE)
    {
        if(strcmp(parameters[0],'cd') == 0)
            exe_cd(parameters);
        if(strcmp(parameters[0],'history') == 0)
            exe_history(parameters);
        if(strcmp(parameters[0],'exit') == 0)
            exe_exit();
        if(strcmp(parameters[0],'mytop') == 0)
            exe_mytop();
    }
    else/*如果不是builin就fork一个子进程来完成*/
    {
        int pid = fork();
        if(pid<0)
        {
            err_sys("fork failed!");
            return -1;
        }
        if(fork()!=0)
        {
        /*父进程需要执行的代码*/
        waitpid(-1,&status,0);
        }
        else
        {
        /*子进程需要执行的相关代码*/
        if(strcmp(parameters[0],'ls') == 0)
            exe_ls(parameters);
        if(strcmp(parameters[0],'vi') == 0)
            exe_vi(parameters);
        if(strcmp(parameters[0],'grep') == 0)
            exe_grep(parameters);
        /*shell支持的后台进程的执行*/
        }
    }
    
}

/*判断是否是内置命令，字符串判断比较使用C库函数strcmp
strcmp函数的原型为
int strcmp(char *str1, const char *str2)
*/
bool built_in(char **parameters[][])
{
    if(strcmp(parameters[0],'cd') == 0)
    return TRUE;
    else if(strcmp(parameters[0],'history') == 0)
    return TRUE;
    else if(strcmp(parameters[0],'exit') == 0)
    return TRUE;
    else if(strcmp(parameters[0],'mytop') == 0)
    return TRUE;
    else 
    return FALSE;
}

/*shell 内置命令*/
/*cd，我自己编写的shell是一个程序，启动我编写的这个shell程序的时候，操作系统为我这个程序分配一个当前工作目录，
而利用chdir系统调用可以移动shell程序的工作目录，也就是成功了移动到了我们目标想传入的路径
参考:unix高级环境编程:P109实例
当前工作目录是进程(这里指就是我这个shell程序的进程)的一个属性，它只影响调用chdir的进程本
身，而不影响其他进程,
所以我在shell中调用而实现shell的内建命令,这也是内建的含义*/
void exe_cd(char **parameters[][])
{
    
    if (chdir(parameters[1]) < 0)
        err_sys("chdir failed");
    printf("cd to %s", parameters[1]);
}
/*history命令：保存shell每次的输入行，打印所需字符串即可;
因为histroy现实最近执行的n条命令，所以倒序即可*/
void exe_history(char **parameters[][], char **cmdhistory[][];)
{
    int n = parameters[1];
    for(int i = historycount-1; i > historycount-1-n ;i--)/*注意这里倒序的下标的边界*/
        printf("%s\n",cmdhistory[i]);
}
/*exit
退出shell的while循环，结束shell的main函数*/
void exe_exit()
{
   exit(0);
}
/*mytop
在minix3系统~/proc文件夹中通过系统调用，
意味着我此时要移动shell的工作目录?
open/read系统调用，在程序只利用I/O进行访问
输出进程信息
输出目标：
- 总体内存大小 read /proc/meminfo文件读 pagesize*total/1024;
- 空闲内存大小 pagesize*free/1024
- 缓存大小 cached*pagesize/1024
- 总体CPU使用占比
    - 得到当前系统各个进程的进程信息，通过/proc/kinfo查看进程和任务数量 */ 
void exe_mytop()
{   
    int pagesize,total,free,largest,cached;
    FILE *fp;
    fp=fopen('/proc/meminfo',"r");
    fscanf(fp, "%d %d %d %d %d", &pagesize,&total,&free,&largest,&cached);
    fclose(fp);
    /*错误处理wrapper*/
    printf("Total memory: %d\n ",pagesize*total/1024);
    printf("Free memory: %d\n",pagesize*free/1024);
    printf("Cached memory: %d\n",pagesize*cached/1024);
    /*难点如何得到所有的proc*/   
}

/*shell非内置命令 Program命令*/
/*运行minix程序,如ls,此时对读取参数的解析需要有变化,而ls本质是打印当前的工作目录*/
void exe_ls(char **parameters[][])
{   
    /*利用execvp调用将minix程序装载到该进程
    并赋予运行参数*/
    /*参数字符串必须以null结尾*/
    execvp("ls",parameters);
}

void exe_vi(char **parameters[][])
{
    execvp("ls",parameters);
}
void exe_bg()
{
    /*子进程，即shellfork出来的那个进程的标准输入、输出映射成/dev/null
    同时，这个子进程调用signal(SIGCHLD,SIG_IGN),使kernel接管此进程
    因此shell可避免调用wait/waitpid直接运行下一条命令*/

}

