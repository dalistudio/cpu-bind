绑定进程或线程到指定的CPU核

    将进程绑定到特定CPU，接口定义如下：
    #include <sched.h>
    int sched_setaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t *set); // 设置CPU亲和性
    int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *set); // 获取CPU亲和性
    
    将线程绑定到特定CPU，接口定义如下：
    #include <pthread.h>
    int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);
    int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *cpuset);
    
    
    参数：
    pid: 进程的id号，如果pid=0,则表示本进程。
    thread: 线程
    cpusetsize: set的大小。
    set: 运行进程的CPU，可以通过以下函数操作set。
        void CPU_ZERO(cpu_set_t *set); // Clears set, so that it contains no CPUs.
        void CPU_SET(int cpu, cpu_set_t *set); // Add CPU cpu to set.
        void CPU_CLR(int cpu, cpu_set_t *set); // Remove CPU cpu from set.
        int CPU_ISSET(int cpu, cpu_set_t *set); // Test to see if CPU cpu is a member of set.
        int CPU_COUNT(cpu_set_t * mask); //Return the number of CPUs in set.
        
 
参考：https://blog.csdn.net/u013511885/article/details/126427521 
 查看cpu有几个核
 命令查看
 ```
$ cat /proc/cpuinfo |grep processor | wc -l
# 或
$ nproc

# 使用cat /proc/cpuinfo可以更详细的查看哪些可以用，哪些不可以用
# 如可以先禁用一个cpu1
$ echo 0 > /sys/devices/system/cpu/cpu1/online
$ cat /proc/cpuinfo
# 再解除禁用，再对比cat /proc/cpuinfo的输出
$ echo 1 > /sys/devices/system/cpu/cpu1/online
$ cat /proc/cpuinfo
# 输出已省略，可自行实验。可以看到，禁用的时候，是没有processor1的

 ```
 
 代码查看
 ```
 #include <unistd.h>
int sysconf(_SC_NPROCESSORS_CONF);/* 返回系统可以使用的核数，但是其值会包括系统中禁用的核的数目，因此该值并不代表当前系统中可用的核数 */
int sysconf(_SC_NPROCESSORS_ONLN);/* 返回值真正的代表了系统当前可用的核数 */

/* 以下两个函数与上述类似 */
#include <sys/sysinfo.h>
int get_nprocs_conf (void);/* 可用核数 */
int get_nprocs (void);/* 真正的反映了当前可用核数 */

 ```
 
 taskset命令
 ```
 #查看一个已存在的任务的亲和性
#注意，该命令显示的结果，是一个十六进制数。如显示32，实际上为0x32，表示亲和性为cpu1,cpu4,cpu5（从cpu0开始计数）
taskset -p <pid>
#执行一个shell命令并设置其亲和性
#注意，这里的21表示的是掩码，十六进制，即0x21,意为cpu0和cpu5
taskset 21 <cmd>

 ```
 
 cpu集(cpu_set_t)
 接口声明
```
       void CPU_ZERO(cpu_set_t *set);
       void CPU_SET(int cpu, cpu_set_t *set);
       void CPU_CLR(int cpu, cpu_set_t *set);
       int  CPU_ISSET(int cpu, cpu_set_t *set);
       int  CPU_COUNT(cpu_set_t *set);
       void CPU_AND(cpu_set_t *destset,
                    cpu_set_t *srcset1, cpu_set_t *srcset2);
       void CPU_OR(cpu_set_t *destset,
                    cpu_set_t *srcset1, cpu_set_t *srcset2);
       void CPU_XOR(cpu_set_t *destset,
                    cpu_set_t *srcset1, cpu_set_t *srcset2);
       int  CPU_EQUAL(cpu_set_t *set1, cpu_set_t *set2);

```

用法举例
```
cpu_set_t set1， set2;
CPU_ZERO(&set1); //清空集合，即set1里不包含任何CPU，本质为所有bit清零
CPU_ZERO(&set2); //清空集合，即set2里不包含任何CPU，本质为所有bit清零
CPU_SET(0, &set1); //将cpu0添加到集合set1中，本质为对应bit置1
CPU_SET(1, &set2); //将cpu1添加到集合set2中，本质为对应bit置1
CPU_CLR(0, &set1); //将cpu0从集合set1中移除，本质为对应bit清零
int ret = CPU_ISSET(1, &set2); //判断cpu1是否在集合set2中，在返回非零，不在返回0
int cnt = CPU_COUNT(&set2); //返回集合set2中的CPU的个数
cpu_set_t result;
CPU_AND(&result, &set1, &set2); //set1和set2的所有bit按位与，结果存入result
CPU_OR(); //按位或
CPU_XOR(); //按位异或
ret = CPU_EQUAL(&set1, &set2); //集合set1和集合set2相等的话，ret为非零，不相等，ret为0

```

动态分配
刚开始接触cpu_set_t时，对_S系列接口有疑问，不明白它存在的意义，明明自己malloc一个cpu_set_t就可以，然后使用各种非_S对其操作，为什么非要有_S系列接口呢？具体原因见glibc源码粗讲解
常用接口声明（在glibc中用本质为宏定义）：
```
       cpu_set_t *CPU_ALLOC(int num_cpus);
       void CPU_FREE(cpu_set_t *set);
       size_t CPU_ALLOC_SIZE(int num_cpus);
       void CPU_ZERO_S(size_t setsize, cpu_set_t *set);
       void CPU_SET_S(int cpu, size_t setsize, cpu_set_t *set);
       void CPU_CLR_S(int cpu, size_t setsize, cpu_set_t *set);
       int  CPU_ISSET_S(int cpu, size_t setsize, cpu_set_t *set);
       int  CPU_COUNT_S(size_t setsize, cpu_set_t *set);
       void CPU_AND_S(size_t setsize, cpu_set_t *destset,
                    cpu_set_t *srcset1, cpu_set_t *srcset2);
       void CPU_OR_S(size_t setsize, cpu_set_t *destset,
                    cpu_set_t *srcset1, cpu_set_t *srcset2);
       void CPU_XOR_S(size_t setsize, cpu_set_t *destset,
                    cpu_set_t *srcset1, cpu_set_t *srcset2);
       int  CPU_EQUAL_S(size_t setsize, cpu_set_t *set1, cpu_set_t *set2);

```

用法举例
```
#define _GNU_SOURCE
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

int
main(int argc, char *argv[])
{
    cpu_set_t *cpusetp;
    size_t size;
    int num_cpus, cpu;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num-cpus>\n", argv[0]);
        exit(EXIT_FAILURE);
    }   

    num_cpus = atoi(argv[1]);
    
    /* 申请一个能够存放num_cpus个CPU的集合 */
    cpusetp = CPU_ALLOC(num_cpus);
    if (cpusetp == NULL) {
        perror("CPU_ALLOC");
        exit(EXIT_FAILURE);
    }   

    /* 获取能存放num_cpus个CPU的集合的大小（单位字节） */
    size = CPU_ALLOC_SIZE(num_cpus);

    /* 清零cpusetp */
    CPU_ZERO_S(size, cpusetp);
    for (cpu = 0; cpu < num_cpus; cpu += 2)
        CPU_SET_S(cpu, size, cpusetp); //将cpu0，cpu2，... 添加到cpusetp集合

    printf("CPU_COUNT() of set:    %d\n", CPU_COUNT_S(size, cpusetp));

    /* 释放cpusetp空间 */
    CPU_FREE(cpusetp);
    exit(EXIT_SUCCESS);
}

```
 
 
 绑定任务到指定CPU
CPU亲和性只是一种倾向性，当绑定的CPU不存在或者存在但是被禁用了，任务会在其他的CPU上执行
设置任务亲和性的接口有：

sched_setaffinity：修改指定pid_t的任务的亲和性
pthread_setaffinity_np：gnu接口，修改指定pthrad_t的任务的亲和性。
pthread_attr_setaffinity_np：gnu接口。创建线程前，通过线程属性结构体控制新线程的亲和性。
注：np的意思为不可移植，即在非gnu的系统上，应该是没有这个接口的。

2.2.1 sched_setaffinity
如果考虑可移植性的话，推荐使用sched_setaffinity()函数将任务绑定到特定CPU执行。

test.c:
```
/* 该代码只是为了验证亲和性，故多数函数均没有检查返回值 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sched.h>

void *func(void *arg)
{
  pthread_setname_np(pthread_self(), (char *)arg);
  while(1)
  {
    char name[128] = {};
    printf("%s\n", (char *)arg);
    /* 死循环耗时，避免打印过快，不同的机器可能需要调整循环次数 */
    for(int i=0; i<1000000000; i++);
  }
  return NULL;
}

int main(void)
{
  pthread_t th1, th2;
  cpu_set_t set;

  /* 初始化一个CPU集合，并向其中添加cpu0和cpu3 */
  CPU_ZERO(&set);
  CPU_SET(0, &set);
  CPU_SET(3, &set);

  /* 设置主线程的亲和性，子线程会继承父线程的亲和性设置(见man pthread_create) */
  int ret = sched_setaffinity(getpid(), sizeof(cpu_set_t), &set);
  if (ret)
  {
    perror("sched_setaffinity");
    return -1;
  }

  pthread_create(&th1, NULL, func, "th1");
  pthread_create(&th2, NULL, func, "th2");

  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  return 0;
}

```

但是，sched_setaffinity函数的不方便之处在于，无法给线程指定亲和性，要用sched_setaffinity给线程指定亲和性，比较麻烦，需要使用到不可移植的函数gettid()。修改test.c如下：

test.c：使用sched_setaffinity修改线程的CPU亲和性。主要逻辑为，主线程设置亲和性为cpu0，启动两个新线程th1和th2，此时从htop可以看到，主线程阻塞，th1和th2交替运行在cpu0上。然后过一段时间后，线程函数自己修改自己的亲和性（th1将自己的亲和性设置为cpu1，th2将自己的亲和性设置为cpu2），此时从htop可以看到，th1到cpu1上运行，th2到cpu2上运行。

```
/* 该代码只是为了验证亲和性，故多数函数均没有检查返回值 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <string.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

void *func(void *arg)
{
  /* 用来计时 */
  int count = 0;
  pthread_setname_np(pthread_self(), (char *)arg);
  while(1)
  {
    char name[128] = {};
    printf("%s\n", (char *)arg);
    /* 死循环耗时，避免打印过快，不同的机器可能需要调整循环次数 */
    for(int i=0; i<1000000000; i++);
    count++;
    /* count小于15时，线程都运行在cpu0上；等于15时，线程会设置自身的亲和性，导致后续时间，亲和性就不再是cpu0了 */
    if(count == 15)
    {
      pid_t tid;
      cpu_set_t set;

      CPU_ZERO(&set);
      /* 获取本线程的pid_t类型的ID号，因为sched_setaffinity的参数类型为pid_t */
      tid = syscall(SYS_gettid);
      if(!strncmp((char *)arg, "th1", strlen("th1")+1))
      {
        CPU_SET(1, &set);
        sched_setaffinity(tid, sizeof(cpu_set_t), &set);
      }
      else
      {
        CPU_SET(2, &set);
        sched_setaffinity(tid, sizeof(cpu_set_t), &set);
      }
    }
  }
  return NULL;
}

int main(void)
{
  pthread_t th1, th2;
  cpu_set_t set;

  CPU_ZERO(&set);
  CPU_SET(0, &set);

  /* 设置主线程亲和性为cpu0，这样的话，默认新线程亲和性也是cpu0 */
  int ret = sched_setaffinity(getpid(), sizeof(cpu_set_t), &set);
  if (ret)
  {
    perror("sched_setaffinity");
    return -1;
  }

  pthread_create(&th1, NULL, func, "th1");
  pthread_create(&th2, NULL, func, "th2");

  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  return 0;
}

```


pthread_setaffinity_np
如果要设置已存在的线程的亲和性，就可以考虑使用pthread_setaffinity_np，尽管pthread_setaffinity_np是不可移植的，是gnu独有的。因为就算是考虑到移植性，想要使用sched_setaffinity，也避免不了要使用gettid函数（该函数也是gnu独有）。与使用sched_setaffinity设置线程亲和性相比，使用pthread_setaffinity_np的另一个好处就是，可以在线程外设置任一线程的亲和性（只要知道pthread_t即可），而sched_setaffinity的gettid，需要在线程函数内调用。

test.c：使用pthread_setaffinity_np，在main函数中，对两个子线程设置其亲和性。前10s亲和性都追随主函数（cpu0），10s后使用pthread_setaffinity_np设置th1亲和性为cpu1，th2亲和性为cpu2，结果见htop截图。
```
/* 该代码只是为了验证亲和性，故多数函数均没有检查返回值 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <string.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

void *func(void *arg)
{
  pthread_setname_np(pthread_self(), (char *)arg);
  while(1)
  {
    char name[128] = {};
    printf("%s\n", (char *)arg);
    /* 死循环耗时，避免打印过快，不同的机器可能需要调整循环次数 */
    for(int i=0; i<1000000000; i++);
  }
  return NULL;
}

int main(void)
{
  pthread_t th1, th2;
  cpu_set_t set;

  CPU_ZERO(&set);
  CPU_SET(0, &set);

  /* 设置主线程亲和性为cpu0，这样的话，默认新线程亲和性也是cpu0 */
  int ret = sched_setaffinity(getpid(), sizeof(cpu_set_t), &set);
  if (ret)
  {
    perror("sched_setaffinity");
    return -1;
  }

  pthread_create(&th1, NULL, func, "th1");
  pthread_create(&th2, NULL, func, "th2");

  /* 前10s，亲和性都是cpu0 */
  sleep(10);
  /* 10s过后，亲和性被修改 */

  /* 移除CPU集合中的cpu0，此时集合中没有任何CPU */
  CPU_CLR(0, &set);
  /* 增加cpu1，此时集合中只有cpu1 */
  CPU_SET(1, &set);
  /* 设置th1的亲和性为cpu1 */
  pthread_setaffinity_np(th1, sizeof(cpu_set_t), &set);

  /* 移除集合中的cpu1，此时集合中没有任何CPU */
  CPU_CLR(1, &set);
  /* 增加cpu2，此时集合中只有cpu2 */
  CPU_SET(2, &set);
  /* 设置th2的亲和性为cpu2 */
  pthread_setaffinity_np(th2, sizeof(cpu_set_t), &set);

  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  return 0;
}

```

pthread_attr_setaffinity_np
创建新线程时，通过属性结构体，控制新线程的亲和性。

test.c:
```
/* 该代码只是为了验证亲和性，故多数函数均没有检查返回值 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <string.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

void *func(void *arg)
{
  pthread_setname_np(pthread_self(), (char *)arg);
  while(1)
  {
    char name[128] = {};
    printf("%s\n", (char *)arg);
    /* 死循环耗时，避免打印过快，不同的机器可能需要调整循环次数 */
    for(int i=0; i<1000000000; i++);
  }
  return NULL;
}

int main(void)
{
  pthread_t th1;
  cpu_set_t set;
  pthread_attr_t attr;

  CPU_ZERO(&set);
  CPU_SET(0, &set);

  /* 设置主线程亲和性为cpu0，这样的话，默认新线程亲和性也应该是cpu0 */
  /* 但是由于新线程创建的时候，使用属性指定了亲和性，就会发现新线程启动之后直接
   * 就在指定的cpu运行了 */
  int ret = sched_setaffinity(getpid(), sizeof(cpu_set_t), &set);
  if (ret)
  {
    perror("sched_setaffinity");
    return -1;
  }

  pthread_attr_init(&attr);
  CPU_ZERO(&set);
  CPU_SET(1, &set);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &set);

  pthread_create(&th1, &attr, func, "th1");

  pthread_join(th1, NULL);

  return 0;
}


```


