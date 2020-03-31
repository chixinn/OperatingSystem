

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
        char *find;
        find=strchr(buf,'\n');
        if(find)
            *find='\0';
        char *tmp = (char*)malloc(sizeof(buf));
        strcpy(tmp,buf);
        history[historycount]=tmp ;
        historycount++;
        struct cmdline l=parse_line(buf);
        execmd(l,history,historycount);

    }
    exit(0);

}



