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
    if (n>historycount){
        printf("n exceed boundary!");
        exit(-1);
    }

    else{
        for(int i = historycount-2; i > historycount-2-n ;i--)/*注意这里倒序的下标的边界,自己测试了一下是正确的*/
            printf("%s\n",history[i]);
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
                waitpid(-1,&status,0);
        }
    }
    else{
        if ((issamestring(">", oneline.parameters[oneline.special_position]) == 1)||(issamestring("<", oneline.parameters[oneline.special_position]) == 1))
            redirection(oneline);
        else{
            char *argv1[MAX_ARGC];
            char *argv2[MAX_ARGC];
        for(int i=0;i<MAX_ARGC;i++){
            argv1[i]=NULL;
            argv2[i]=NULL;
        }
        for (int i = 0; i < oneline.special_position; i++) {
            argv1[i] = oneline.parameters[i];
        }
        argv1[oneline.special_position] = NULL;//execvp使用；提取特殊命令钱的programA 和program B

    /*在测试样例中知道了最后一个置为NULL的重要性*/
        int j=oneline.special_position+1;
        for (int i = 0; i < oneline.argc - oneline.special_position-1; i++) {
            argv2[i] = oneline.parameters[j];
            j++;
    }
            argv2[oneline.argc-oneline.special_position] = NULL;//execvp使用；提取特殊命令钱的programA 和program B//
            int status;
            if(fork()==0){
                pipeline(argv1,argv2);
                exit(0);
            }
            else{
               waitpid(-1,&status,0);
            }
            

        }
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
void redirection(struct cmdline oneline) {
    /*直接在重定向里建立fork()*/

    /*此处我先实现输入输出重定向操作再执行ls，本来ls的输出是result.txt ，即ls程序不知道result.txt的存在*/
    /*提取重定向钱的命令参数*/
    char *argv1[MAX_ARGC];
    char *argv2[MAX_ARGC];
    for(int i=0;i<MAX_ARGC;i++){
        argv1[i]=NULL;
        argv2[i]=NULL;
    }
    for (int i = 0; i < oneline.special_position; i++) {
        argv1[i] = oneline.parameters[i];
    }
    argv1[oneline.special_position] = NULL;//execvp使用；提取特殊命令钱的programA 和program B

    /*在测试样例中知道了最后一个置为NULL的重要性*/
    int j=oneline.special_position+1;
    for (int i = 0; i < oneline.argc - oneline.special_position-1; i++) {
        argv2[i] = oneline.parameters[j];
        j++;
    }
    argv2[oneline.argc-oneline.special_position] = NULL;//execvp使用；提取特殊命令钱的programA 和program B//


    int pid, status, fd;


    if (issamestring(">", oneline.parameters[oneline.special_position]) == 1) {
            fd = dup(1);//复制标准输出,此时dup返回的是最新空闲,即让一个最新空闲也指向1
            close(1);//关闭标准输出
     }
    else {
        fd = dup(0);//复制，此时最新空闲的是fd
        close(0);//关闭标准输入
        /*先看文件是否存在，存在就覆盖写，不存在就创建写入*/}
        /*fd要设置成全局变量*/
    pid = fork();
    if (pid < 0) {
            printf("fork error!");
            exit(1);
    }
    else if (pid == 0) {
        if(issamestring(">", oneline.parameters[oneline.special_position]) == 1){
            open(argv2[0], O_WRONLY | O_CREAT | O_TRUNC);
        }
        else{
            open(argv2[0], O_RDONLY);
        }
        execvp(argv1[0],argv1);
    }
    else{
        waitpid(-1, &status, 0);
        if (issamestring(">", oneline.parameters[oneline.special_position]) == 1)
            close(1);
        else
            close(0);
        dup(fd);//将当前最小的文件描述符1，指向fd指向的；
        close(fd);//不让fd再指向1，即恢复最开始的状态
    }
    /*这里注意文件标识符的恢复问题*/
    /*文件表示符的恢复应该放在父进程里"执行*/
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
            if(oneline.is_background!=1)
                waitpid(-1,&status,0);
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
        exit(0);

        /*我个人感觉原来ls的执行命令搭载的很好了*/
        
    }
}
