#include "stdio.h"
#include "string.h"
#include "unix.h"

#define TRUE 1
#define MAXLINE 1024
#define MAXARGS 32
#define MAXHISTORYLINE 64
#define MASARGLEN 32


int main()
{
    int historycount = 0;
    char *cmdline[MAXLINE];
    char **parameters[MAXARGS][MASARGLEN];
    memset(parameters,0,sizeof(parameters));
    char **cmdhistory;
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
    parameters = parse_line(line);

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
    char **parameters[MAXARGS][MASARGLEN];
    char *token = strtok(str, s);
    while(token != NULL)
    {
        parameters[i] = strtok(NULL, s);/*获取参数列表*/
        i++;
    }
    return parameters;
}





int built_in_or_not(char *cmdline[0]);

void execute_cmd()
{
 if(cmd == builtin_cmd)
    {
        /*进行相应的系统调用*/

    }
    /*如果不是builin就fork一个子进程来完成*/
    else
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
