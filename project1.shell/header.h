#include <stdio.h>
#include"string.h"
#include<unistd.h>
/*包含了 STDIN_FILENO;STDOUT_FILENO;STDERR_FILENO*/
#include<stdlib.h>
#include<fcntl.h>/*oflag参数*/
#include<signal.h>
#include <dirent.h>


#define MAXLINE 1024
#define STD_INPUT 0
#define STD_OUTPUT 1
#define MAX_ARGC 5
struct cmdline
{
    char *buf;
    char *parameters[MAXLINE];
    int argc;
    int is_background;
    int is_special;
    int special_position;

};

int nr_total;

struct cmdline parse_line(char buf[]);
void execmd(struct cmdline l,char *history[],int historycount);
void exe_mytop();
int issamestring(const char *a,const char *b);
void exe_cd(char *parameters[],char* history[],int historycount);
void exe_history(char *parameters[],char* history[],int historycount);
void exe_ls(struct cmdline oneline);
void redirection(struct cmdline oneline);
void  exe_grep(struct cmdline oneline);
void exe_vi(struct cmdline oneline);
void pipeline(char *process1[],char *process2[]);