/*
*gcc process_test.c
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>

/*
    将进程绑定到特定CPU，接口定义如下：
    #include <sched.h>
    int sched_setaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t *set); // 设置CPU亲和性
    int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *set); // 获取CPU亲和性
    
    参数：
    pid: 进程的id号，如果pid=0,则表示本进程。
    cpusetsize: set的大小。
    set: 运行进程的CPU，可以通过以下函数操作set。
        void CPU_ZERO(cpu_set_t *set); // Clears set, so that it contains no CPUs.
        void CPU_SET(int cpu, cpu_set_t *set); // Add CPU cpu to set.
        void CPU_CLR(int cpu, cpu_set_t *set); // Remove CPU cpu from set.
        int CPU_ISSET(int cpu, cpu_set_t *set); // Test to see if CPU cpu is a member of set.
        int CPU_COUNT(cpu_set_t * mask); //Return the number of CPUs in set.
        
    编译：
    gcc process_test.c -o process_test
    
    测试：
    ps -elf | grep process_test
    查看获取PID值后再：
    top -p PID
    输入f，选择P选择(已到P处，空格选择)，按ESC退出
    然后观察  P 的变化，在cpu0、cpu1、cpu2、cpu3之间不停的切换。
*/

// 浪费时间
void WasteTime(void)
{
    int abc = 1000;
    int tmp = 0;
    while(abc--)
        tmp = 10000*10000;
    sleep(1);
}

// 主程序
int main(int argc, char *argv[])
{
    cpu_set_t cpu_set; // 声明变量
    while(1)
    {
        CPU_ZERO(&cpu_set); // 清零
        CPU_SET(0, &cpu_set); // 设置为cpu0
        if(sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0) // 调度，设置亲和性关系
            perror("sched_setaffinity");
            
        WasteTime(); // 浪费时间
        
        CPU_ZERO(&cpu_set); // 清零
        CPU_SET(1, &cpu_set); // 设置为cpu1
        if(sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0) // 调度，设置亲和性关系
            perror("sched_setaffinity");
            
        WasteTime(); // 浪费时间
        
        CPU_ZERO(&cpu_set); // 清零
        CPU_SET(2, &cpu_set); // 设置为cpu2
        if (sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0) // 调度，设置亲和性关系
            perror("sched_setaffinity");
            
        WasteTime(); // 浪费时间
        
        CPU_ZERO(&cpu_set); // 清零
        CPU_SET(3, &cpu_set); // 设置为cpu3
        if(sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0) // 调度，设置亲和性关系
            perror("sched_setaffinity");
            
        WasteTime(); // 浪费时间
    }
    return 0;
}


