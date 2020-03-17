#include "stdio.h"
#include "string.h"
#define TRUE 1
#define MAXLINE 1024
#define MAXARGS 128
#define MAXHISTORYLINE 64


int main()
{
    int historycount = 0;
    string cmdhistory[MAXHISTORYLINE][MAXLINE];
    memset(cmdhistory,0,sizeof(cmdhistory));

while(True)
{   
    type_prompt();
    char *cmdline[MAXLINE];
    
    scanf("%s",&cmdline);//这个地方用gets可以吗?
    
    read_command(cmd,parameters);/*不断的接受键盘并反馈*/    
    /*Shell 主要是为用户提供了一个命令解释器，接收用户命令主要是调用相应的应用程序，
        实现Shell支持后台进程的运行。*/
    if(cmd == builtin_cmd)
    {
        /*进行相应的系统调用*/

    }
    /*如果不是builin就fork一个子进程来完成*/
    else
    {
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
return 0;
}

void type_prompt()
{
    printf("$");
}

/*shell将输入行分解成单词序列*/
void parseline(char *cmdline,char *parameters)
{

}
int built_in_or_not(char *cmdline[0])