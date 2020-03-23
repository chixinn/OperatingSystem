#include <stdio.h>
#include"string.h"
#include<unistd.h>
/*包含了 STDIN_FILENO;STDOUT_FILENO;STDERR_FILENO*/
#include<stdlib.h>
#include<fcntl.h>/*oflag参数*/


#define MAXLINE 1024
#define STD_INPUT 0
#define STD_OUTPUT 1

struct cmdline
{
    char *buf;
    char *parameters[MAXLINE];
    int argc;
    int is_background;
    int is_special;
    int special_position;

};

struct cmdline parse_line(char buf[]);
void execmd(struct cmdline l,char *history[],int historycount);
void exe_mytop();
int issamestring(char *a,char *b);
void exe_cd(char *parameters[],char* history[],int historycount);
void exe_history(char *parameters[],char* history[],int historycount);
void exe_ls(struct cmdline oneline);
void redirection(struct cmdline oneline);
void  exe_grep(struct cmdline oneline);
void exe_vi(struct cmdline oneline);
void pipeline(char *process1[],char *process2[]);

int main()
{
    int historycount = 0;
    char *history[MAXLINE];
    char buf[MAXLINE];
    memset(buf,0,sizeof(buf));
    memset(history,0,sizeof(history));
    while(1)
    {
        printf("$");
        /*fgets从文件标准输入一次读一行，当渐入文件结束符（ctrl+D),fgets返回一个null指针*/
        fgets(buf,MAXLINE,stdin);
        char *tmp = (char*)malloc(sizeof(buf));
        strcpy(tmp,buf);
        history[historycount]=tmp ;
        historycount++;
        struct cmdline l=parse_line(buf);
        printf("%d\n",l.argc);
        printf("%d\n",l.is_background);
        printf("%d\n",l.is_special);
        printf("%d\n",l.special_position);

         execmd(l,history,historycount);


        //printf("%lu",strlen(l.parameters[l.argc-1]));//之前一直sizeof一个指针，所以会一直返回8。。。太他妈神奇了


        break;//防止提示
    }

    return 0;
}

struct cmdline parse_line(char buf[])
{
    int i=0;
    char *str=buf;
    struct cmdline res;
    res.buf=buf;
    res.is_special=0;
    res.special_position=-1;
    char *token = strtok(str, " ");
    res.parameters[i]=token;
    while(token !=NULL)
    {
        token = strtok(NULL, " ");
        i++;
        res.parameters[i]=token;
        if(token!=NULL){
            if((issamestring("|",token))||(issamestring(">",token))||(issamestring("<",token))||(issamestring(">>",token))){
                res.is_special=1;
                res.special_position=i;
            }
        }
    }
    res.argc=i;//存参数个数
    res.parameters[res.argc]=NULL;//也将末尾置为null
    if(issamestring("&",res.parameters[res.argc-1]) == 1)//tokens返回的最后一个字符串与想象中应该是多了一个换行符
        res.is_background=1;
    else
        res.is_background=0;

    return res;
}
int issamestring(char *a,char *b)
{
    for(int i=0;a[i]!='\0';i++){
        if(a[i]!=b[i])
            return 0;
    }
    return 1;
}
int built_in(char *para){
    if(issamestring("cd",para)==1)
        return 1;
    if(issamestring("history",para)==1)
        return 1;
    if(issamestring("exit",para)==1)
        return 1;
    if(issamestring("exit",para)==1)
        return 1;
    else
        return 0;
}
void execmd(struct cmdline l,char *history[],int historycount){
    if(built_in(l.parameters[0])==1)
    {
        if(issamestring("cd",l.parameters[0])==1)
            exe_cd(l.parameters,history,historycount);
        if(issamestring("history",l.parameters[0])==1)
            exe_history(l.parameters,history,historycount);
        if(issamestring("exit",l.parameters[0])==1)
            exit(0);
        if(issamestring("mytop",l.parameters[0])==1)
            exe_mytop();
    }
    else{/*如果不是内置命令的文件*/
        if(issamestring("vi",l.parameters[0])==1)
            exe_vi(l);
        else if(issamestring("ls",l.parameters[0])==1)
            exe_ls(l);
        else if(issamestring("grep",l.parameters[0])==1)
            exe_grep(l);
        else
            printf("no %s",l.buf);
    }
}


void exe_cd(char *parameters[],char* history[],int historycount){
    if(chdir(parameters[1])<0){
        printf("chdir failed");
    }
      //err_sys是作者自己写的函数,而不是C库函数
    else
         printf("cd to %s succeed",parameters[1]);
}//测试过了，注意路径填写规范
void exe_history(char *parameters[],char* history[],int historycount){
    int n = atoi(parameters[1]);
    if (n>historycount)
        printf("n exceed boundary!");

    for(int i = historycount-1; i > historycount-1-n ;i--)/*注意这里倒序的下标的边界,自己测试了一下是正确的*/
        printf("%s\n",history[i]);

}
void exe_mytop(){
    int pagesize,total,free,largest,cached;
    FILE *fp;
    fp=fopen("/proc/meminfo","r");/*打开的不是路径而是文件描述符*/
    if(fp==NULL)
        printf("file open error");
    else{

        fscanf(fp, "%d %d %d %d %d", &pagesize,&total,&free,&largest,&cached);
        fclose(fp);
        /*错误处理wrapper*/
        printf("Total memory: %f\n ",pagesize*total*1.0/1024);
        printf("Free memory: %f\n",pagesize*free*1.0/1024);
        printf("Cached memory: %f\n",pagesize*cached*1.0/1024);
        /*难点如何得到所有的proc*/
    }
}

/*Program命令*/
/*运行程序：fork出子，在子进程execvp装载，父进程等子进程结束*/
/*重定向，在子进程装载程序前进行。*/


void exe_ls(struct cmdline oneline){
    if(oneline.is_special==0){
        int pid,status;
        pid=fork();
        if(pid<0)
            printf("fork error!");
        else if(pid ==0){
            execvp("ls",oneline.parameters);
        }
        else{
            if(oneline.is_background==1)
                exit(0);
            waitpid(-1,&status,0);
            exit(0);
        }
    }
    else{
        redirection(oneline);
    }
}
void exe_vi(struct cmdline oneline)
{
    int pid,status;
    pid=fork();
    if(pid<0){
        printf("fork error!");
        exit(1);
    }
    if(pid ==0){
        execvp("vi",oneline.parameters);
    }
    else{
        if(oneline.is_background==1)
            exit(0);
        waitpid(-1,&status,0);
        exit(0);
    }
}
void redirection(struct cmdline oneline) {
    /*直接在重定向里建立fork()*/

    /*此处我先实现输入输出重定向操作再执行ls，本来ls的输出是result.txt ，即ls程序不知道result.txt的存在*/
    /*提取重定向钱的命令参数*/
    char *argv1[oneline.special_position + 1];
    char *argv2[oneline.argc - oneline.special_position];
    for (int i = 0; i < oneline.special_position; i++) {
        argv1[i] = oneline.parameters[i];
    }
    argv1[oneline.special_position] = NULL;//execvp使用；提取特殊命令钱的programA 和program B
    /*在测试样例中知道了最后一个置为NULL的重要性*/
    for (int i = 0; i < oneline.special_position; i++) {
        argv2[i] = oneline.parameters[i];
    }
    argv2[oneline.argc] = NULL;//execvp使用；提取特殊命令钱的programA 和program B


    int pid, status, fd;
    if (issamestring("|", oneline.parameters[oneline.special_position]) == 1) {
        pipeline(argv1,argv2);
    }
    else {
        if (issamestring(">", oneline.parameters[oneline.special_position]) == 1) {
            fd = dup(1);//复制标准输出,此时dup返回的是最新空闲,即让一个最新空闲也指向1
            close(1);//关闭标准输出
            open(oneline.parameters[oneline.special_position + 1], O_WRONLY | O_CREAT | O_TRUNC);
        }
        if (issamestring("<", oneline.parameters[oneline.special_position]) == 1) {
            fd = dup(0);//复制，此时最新空闲的是fd
            close(0);//关闭标准输入
            open(oneline.parameters[oneline.special_position + 1], O_WRONLY | O_CREAT | O_TRUNC);
            /*先看文件是否存在，存在就覆盖写，不存在就创建写入*/
        }
        /*fd要设置成全局变量*/
        pid = fork();
        if (pid < 0) {
            printf("fork error!");
            exit(1);
        }
        if (pid == 0) {
            if (issamestring("ls", oneline.parameters[oneline.special_position]) == 1)
                execvp("ls", argv1);
            if (issamestring("grep", oneline.parameters[oneline.special_position]) == 1)
                execvp("grep", argv1);
        } else {
            /*这里注意文件标识符的恢复问题*/
            /*文件表示符的恢复应该放在父进程里"执行*/
            if (oneline.is_background == 1) {
                if (issamestring(">", oneline.parameters[oneline.special_position]) == 1)
                    close(1);
                else
                    close(0);

                dup(fd);//将当前最小的文件描述符1，指向fd指向的；
                close(fd);//不让fd再指向1，即恢复最开始的状态
                exit(0);
            } else {
                waitpid(-1, &status, 0);
                if (issamestring(">", oneline.parameters[oneline.special_position]) == 1)
                    close(1);
                else
                    close(0);

                dup(fd);//将当前最小的文件描述符1，指向fd指向的；
                close(fd);//不让fd再指向1，即恢复最开始的状态
                exit(0);
            }
        }
    }
}
void exe_grep(struct cmdline oneline){
    if(oneline.is_special==0){
        int pid,status;
        pid=fork();
        if(pid<0)
            printf("fork error!");
        else if(pid ==0){
            execvp("grep",oneline.parameters);
        }
        else{
            if(oneline.is_background==1)
                exit(0);
            waitpid(-1,&status,0);
            exit(0);
        }
    }
    else{
        redirection(oneline);
    }
}
void pipeline(char *process1[],char *process2[]){
    /*在fork前先用管道穿件管道描述符
     * 创建不必要的读写端
     * 对于每个子进程我需要的操作即做1-1映射*/
    int fd[2];
    pipe(fd);
    int pid,status;
    pid=fork();

    if(pid<0){
        printf("fork error!");
        exit(1);
    }
    else if(pid==0) {
        close(fd[0]);//不用从管道读
        close(STD_OUTPUT);//关闭标准输出
        dup(fd[1]);//把管道出指向标准读
        close(fd[1]);//不再需要管道了
        execvp(process1[0], process1);
        exit(0);
    }
    else{
        close(fd[1]);
        close(STD_INPUT);
        dup(fd[0]);
        close(fd[0]);
        waitpid(-1,&status,0);
        execvp(process2[0], process2);
        /*我个人感觉原来ls的执行命令搭载的很好了*/
        exit(0);
    }
}

