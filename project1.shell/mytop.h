#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <limits.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include <dirent.h>

typedef int endpoint_t;
typedef uint64_t u64_t;
typedef long unsigned int vir_bytes;

const char *cputimenames[] = { "user", "ipc", "kernelcall" };
#define CPUTIMENAMES (sizeof(cputimenames)/sizeof(cputimenames[0]))
#define CPUTIME(m, i) (m & (1L << (i)))

#define MAX_NR_TASKS	1023
#define NR_TASKS	  5
#define IDLE    ((endpoint_t) -4) /* runs when no one else can run */
#define KERNEL  ((endpoint_t) -1)

#define SELF ((endpoint_t) 0x8ace)
#define _MAX_MAGIC_PROC (SELF)
#define _ENDPOINT_GENERATION_SIZE (MAX_NR_TASKS+_MAX_MAGIC_PROC+1)
#define _ENDPOINT_P(e) \
	((((e)+MAX_NR_TASKS) % _ENDPOINT_GENERATION_SIZE) - MAX_NR_TASKS)
#define  SLOT_NR(e) (_ENDPOINT_P(e) + NR_TASKS)

#define  USED		0x1
#define  IS_TASK	0x2
#define  IS_SYSTEM	0x4
#define  BLOCKED	0x8

#define TYPE_TASK	'T'
#define TYPE_SYSTEM	'S'
#define STATE_RUN	'R'

unsigned int nr_procs, nr_tasks;
int nr_total=0;
int cputimenode=1;
int slot_a=0;

struct proc {
    int p_flags;
    endpoint_t p_endpoint;
    pid_t p_pid;
    u64_t p_cpucycles[CPUTIMENAMES];
    int p_priority;
    endpoint_t p_blocked;
    time_t p_user_time;
    vir_bytes p_memory;
    uid_t p_effuid;
    int p_nice;
};
struct tp {
    struct proc *p;
    u64_t ticks;
};
struct proc *proc = NULL, *prev_proc = NULL;

void print_memifo(void);
void getkinfo(void);
void print_cpu(void);
void get_procs(void);
void print_procs(struct proc *prev_proc,struct proc *proc,int cputimemode);
void parse_dir(void);
void parse_file(pid_t pid);
u64_t cputicks(struct proc *p1, struct proc *p2, int timemode);
static inline u64_t make64(unsigned long lo, unsigned long hi)
{
    return ((u64_t)hi << 32) | (u64_t)lo;
}