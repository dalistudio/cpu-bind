/*
*gcc thread_test.c -lpthread
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

/*
    将线程绑定到特定CPU，接口定义如下：
    #include <pthread.h>
    int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset); // 设置CPU亲和性
    int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *cpuset); // 获取CPU亲和性
    
    参数：
    thread: 线程
    cpusetsize: set的大小。
    set: 运行进程的CPU，可以通过以下函数操作set。
        void CPU_ZERO(cpu_set_t *set); // Clears set, so that it contains no CPUs.
        void CPU_SET(int cpu, cpu_set_t *set); // Add CPU cpu to set.
        void CPU_CLR(int cpu, cpu_set_t *set); // Remove CPU cpu from set.
        int CPU_ISSET(int cpu, cpu_set_t *set); // Test to see if CPU cpu is a member of set.
        int CPU_COUNT(cpu_set_t * mask); //Return the number of CPUs in set.
        
    编译：
    gcc thread_test.c -o thread_test
    
    测试：
    ps -elf | grep thread_test
    查看获取PID值后再：
    top -H -p PID
    输入f，选择P选择(已到P处，空格选择)，按ESC退出
    然后观察  P 的变化，可以看到主线程在cpu0一直保持不变；一个线程cpu1、cpu2之间不停的切换；
    另一个线程一直保持在cpu3不变。
*/

// 浪费时间
void WasteTime(void)
{
    int abc = 1000;
    int temp = 0;
    while(abc--)
        temp = 10000*10000;
    sleep(1);
}

// 线程1
void *thread_func1(void *param)
{
    cpu_set_t cpu_set;
    while(1)
    {
        CPU_ZERO(&cpu_set); // 清空
        CPU_SET(1, &cpu_set); // 设置 为cpu1
        if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set),&cpu_set) < 0) // 调度，设置亲和性关系
            perror("pthread_setaffinity_np");

        WasteTime(); // 浪费时间
        
        CPU_ZERO(&cpu_set);
        CPU_SET(2, &cpu_set); // 设置 为cpu2
        if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set),&cpu_set) < 0) // 调度，设置亲和性关系
            perror("pthread_setaffinity_np");

        WasteTime(); // 浪费时间
    }
}

// 线程2
void *thread_func2(void *param)
{
    cpu_set_t cpu_set;
    while(1)
    {
        CPU_ZERO(&cpu_set); // 清空
        CPU_SET(3, &cpu_set); // 设置 为cpu3
        if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set),&cpu_set) < 0) // 调度，设置亲和性关系
            perror("pthread_setaffinity_np");

        WasteTime(); // 浪费时间
    }
}

// 主程序
int main(int argc, char *argv[])
{
    // 可以执行下面命令禁用看看，1=开启，0=禁用
    // #echo 0 > /sys/devices/system/cpu/cpu1/online
    printf("CPU Core Total=%ld\n",sysconf(_SC_NPROCESSORS_CONF)); // 不一定准，可能存在禁用的CPU核
    printf("CPU Core Usable=%ld\n",sysconf(_SC_NPROCESSORS_ONLN)); // 可用的CPU核
    
    pthread_t my_thread;
    cpu_set_t cpu_set;
    
    CPU_ZERO(&cpu_set); // 清空
    CPU_SET(0, &cpu_set); // 设置 为cpu0
    if (sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0) // 调度，设置关系，绑定不变
        perror("sched_setaffinity");

    if (pthread_create(&my_thread, NULL, thread_func1,NULL) != 0) // 创建线程1，不停切换
        perror("pthread_create");

    if (pthread_create(&my_thread, NULL, thread_func2,NULL) != 0)  // 创建线程2，绑定不变
        perror("pthread_create");

    while(1) // 死循环
        WasteTime(); // 浪费时间

    pthread_exit(NULL); // 退出线程
}

