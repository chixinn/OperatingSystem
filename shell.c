#include <stdio.h>
#include "string.h"
#include "apue.h"
#include <unistd.h>

#define TRUE 1
#define MAXLINE 1024
#define MAXLINES 64
#define MAXARGS 32
#define MAXHISTORYLINE 64
#define MASARGLEN 32

/*目前版本都未实现错误处理的wrapper和考虑C语言中字符串数组初始化的内存问题*/
int main()
{
    int historycount = 0;
    char *cmdline[MAXLINE];
    char **parameters[MAXARGS][MASARGLEN];
    memset(parameters,0,sizeof(parameters));
    char **cmdhistory[MAXLINES][MAXLINE];
    memset(cmdhistory,0,sizeof(cmdhistory));


while(True)
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

void type_prompt()
{
    printf("$");
}

/*读入行，使用getline函数*/
char *read_cmd(void)
{
    char *line = NULL;
    sszie_t bufsize = 0;
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
            perror("fork failed!");
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
        execve(command,parameters,0);/*shell支持的后台进程的执行*/
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
通过minix3系统~/proc文件夹中 open/read系统调用输出进程信息
输出目标：
- 总体内存大小
- 空闲内存大小
- 缓存大小*/
void exe_mytop()
{
    
}