#include"shell.h"
struct cmdline parse_line(char buf[])
{
    int i=0;
    char *str=buf;
    struct cmdline res;
    res.buf=buf;
    res.is_special=0;
    res.special_position=-1;
    res.cmdnumber=0;
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
                res.cmdnumber++;
            }
        }
    }
    res.argc=i;//存参数个数
    res.parameters[res.argc]=NULL;//也将末尾置为null
    if(issamestring("&",res.parameters[res.argc-1]) == 1)//tokens返回的最后一个字符串与想象中应该是多了一个换行符,那是因为fgets它多读了一个换行符
        res.is_background=1;
    else
        res.is_background=0;

    return res;
}
int issamestring(const char *a,const char *b)
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
    if(issamestring("mytop",para)==1)
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




