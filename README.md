绑定进程或线程到指定的CPU核

    将进程绑定到特定CPU，接口定义如下：
    #include <sched.h>
    int sched_setaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t *set); // 设置CPU绑定关系
    int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *set); // 获取CPU绑定关系
    
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
