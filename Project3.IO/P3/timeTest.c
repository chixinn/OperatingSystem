#include<stdio.h>
#include<time.h>
int main(){
    time_t starttime=time(NULL);
    printf("%Ld\n",starttime);
    int status=0;
    if(fork()==0){
        sleep(30);
        exit(1);
    }
    else{
        wait(&status);
        time_t endtime=time(NULL);
        printf("%Ld\n",endtime);
        long spendtime=endtime-starttime;
        printf("spendtime = %ld\n",spendtime);
        exit(0);
    }
}