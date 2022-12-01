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

