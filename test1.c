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

