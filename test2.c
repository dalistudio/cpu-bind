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

