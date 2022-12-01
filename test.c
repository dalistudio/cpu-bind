/* 该代码只是为了验证亲和性，故多数函数均没有检查返回值 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sched.h>

// gcc test.c -pthread -o test
// ps aux | grep test
// htop -t -p PID

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

