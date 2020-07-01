#include<stdio.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include <time.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
// block为块大小 
// blocksize为块扫描方式
//C语言计算时间的差值。
#define iteration 10000//原来太小了就一直INF
#define MBFRACTOR 1024*1024
#define KBFRACTOR 1024
#define fileSize   6*MBFRACTOR//总fileSize为300MB
#define bufferSize 6*KBFRACTOR//buffersize 6KB
char buffer[bufferSize]="This is a test, and I dont know what to use these for";

void write_file(int blocksize, int isrand,char *filepath){
    int fd=0;

    fd=open(filepath,O_CREAT | O_RDWR | O_SYNC,0755);
    /*成功*/
    if(fd<0){
        printf("Open error!");
        return;
    }
    else{
        /*ssize_t write(int fd,const void*buf,size_t count);
        参数说明：
        fd:是文件描述符（输出到command line，就是1）
        buf:通常是一个字符串，需要写入的字符串
        count：是每次写入blocksize的字节数*/
       for(int i=0;i<iteration;i++){

            write(fd,buffer,blocksize);
            if(isrand)
                lseek(fd,rand() % fileSize,SEEK_SET);//利用随机函数写到文件的任意一个位置
        //如果是随机
       }
        // lseek(fd,0,SEEK_SET);//如果读到末尾则从文件开头开始读。
        }
        //顺序读写时：默认文件指针自由移动
    }
    

void read_file(int blocksize, int isrand,char *filepath){
    int fd=0;

    fd=open(filepath,O_CREAT | O_RDWR | O_SYNC);
    /*成功*/
    if(fd<0){
        printf("Open error!");
        return;
    }
    else{
        /*ssize_t write(int fd,const void*buf,size_t count);
        参数说明：
        fd:是文件描述符（输出到command line，就是1）
        buf:通常是一个字符串，需要写入的字符串
        count：是每次写入blocksize的字节数*/
       for(int i=0;i<iteration;i++){

            read(fd,buffer,blocksize);
            if(isrand)
            lseek(fd,rand() % fileSize,SEEK_SET);//利用随机函数写到文件的任意一个位置
        //如果是随机
       }
         lseek(fd,0,SEEK_SET);//如果读到末尾则从文件开头开始读。
        }
        //顺序读写时：默认文件指针自由移动
}
//计算时间差，在读或写操作前后分别取系统时间，然后计算差值即为所花时间
long get_time_left(long long starttime ,long long endtime){
    long spendtime=endtime-starttime;
    return spendtime;
}

/*main*/
/*首先创建和命明文件，通过循环执行read_file和write_file测试读写的差异
测试blocksize和concurrency对测试读写速度的影响*/
int main(){
    srand((unsigned)time(NULL));//随机种子
   // FILE *fpRAM,*fpDISK;//文件句柄
    int concurrency=7;//并发的进程的个数
    char *filepathDISK="/usr/mytest.txt";
    char *filepathRAM="/root/myram/mytest.txt";
    char *filepathDISKNUM[20]={"/usr/mytest1.txt","/usr/mytest2.txt","/usr/mytest3.txt","/usr/mytest4.txt","/usr/mytest5.txt","/usr/mytest6.txt","/usr/mytest7.txt","/usr/mytest8.txt","/usr/mytest9.txt","/usr/mytest10.txt","/usr/mytest11.txt","/usr/mytest12.txt","/usr/mytest13.txt","/usr/mytest14.txt","/usr/mytest15.txt","/usr/mytest16.txt","/usr/mytest17.txt","/usr/mytest18.txt","/usr/mytest19.txt","/usr/mytest20.txt"};
    char *filepathRAMNUM[20]={"/root/myram/mytest1.txt","/root/myram/mytest2.txt","/root/myram/mytest3.txt","/root/myram/mytest4.txt","/root/myram/mytest5.txt","/root/myram/mytest6.txt","/root/myram/mytest7.txt","/root/myram/mytest8.txt","/root/myram/mytest9.txt","/root/myram/mytest10.txt","/root/myram/mytest11.txt","/root/myram/mytest12.txt","/root/myram/mytest13.txt","/root/myram/mytest14.txt","/root/myram/mytest15.txt","/root/myram/mytest16.txt","/root/myram/mytest17.txt","/root/myram/mytest18.txt","/root/myram/mytest19.txt","/root/myram/mytest20.txt"};
    int i;
    time_t starttime,endtime;
    int status = 0;
    long spendtime;
    int blocksize=64;//64B
    int nameIter=0;
    char *nameStr;
    for(int j=0;j<bufferSize; j=j+6){
        strncat(buffer,"haha haha!\n",6);
    }
    //fpRAM=fopen(filepathRAM,"w+");
  //  fpDISK=fopen(filepathDISK,"w+");
  
    do{
    starttime=time(NULL);
   // printf("starttime:%lld",starttime);
   
       for( i=0;i<concurrency;i++){
        if(fork()==0){
        //随机写
        int iter=0;
       
    //   write_file(blocksize,1,filepathDISKNUM[i]);
    //   write_file(blocksize,0,filepathDISKNUM[i]);
       write_file(blocksize,0,filepathRAMNUM[i]);
    //  write_file(blocksize,1,filepathRAMNUM[i]);

    /*write*/

    //    read_file(blocksize,1,filepathDISKNUM[i]);
    //   read_file(blocksize,0,filepathDISKNUM[i]);
    //    read_file(blocksize,0,filepathRAMNUM[i]);
    //    read_file(blocksize,1,filepathRAMNUM[i]);    
    

        exit(1);
        } 
    }
    while(wait(NULL)!=-1){}

    endtime=time(NULL);
    printf("endtime:%lld\n",endtime);
    spendtime=get_time_left(starttime,endtime);//操作所花费的时间
    printf("spendtime:%ld\n",spendtime);
   double throughput=1.0*blocksize/spendtime;
    printf("blocksize = %d,throughput = %lf,concurrency = %d\n",blocksize,throughput,concurrency);
    //printf("plz input another blocksize\n");
    //printf("the blocksize = %d\n",blocksize);

    i=0;
   
    blocksize=blocksize*2;
    // concurrency=15;

    
    }while( blocksize<=2100);

  /*等待子进程完成后，获取计算时间,计算读写操作所花时间，延时，吞吐量*/
  return 0;
    
}
