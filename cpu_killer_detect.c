// Created by 22674 on 2025/8/30.

#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/catlog.h"

// 进程信息结构体：存储PID、CPU使用率、进程名
typedef struct {
    pid_t pid;               // 进程ID
    double cpu_usage;        // CPU使用率（0~100%）
    char comm[256];          // 进程名（从/proc/[PID]/stat第2字段获取）
} ProcessInfo;


//返回值：系统总CPU时间（单位：jiffies，1jiffy≈1/Hz，Hz为系统时钟频率，ARM Linux通常为100/250/1000）
static unsigned long long get_total_cpu_time(void) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        perror("fopen /proc/stat failed");
        return 0;
    }

    // /proc/stat首行格式：cpu  user  nice  system  idle  iowait  irq  softirq  steal...
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(fp);

    // 总CPU时间 = 用户态 + 低优先级用户态 + 内核态 + 空闲态 + I/O等待 + 硬中断 + 软中断 + 虚拟化窃取时间
    return user + nice + system + idle + iowait + irq + softirq + steal;
}


// 参数：pid-目标进程ID，utime-输出用户态CPU时间，stime-输出内核态CPU时间，comm-输出进程名
// 返回值：0=成功，-1=失败
static int get_process_cpu_time(pid_t pid, unsigned long long *utime, unsigned long long *stime, char *comm) {
    char proc_path[64];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/stat", pid);

    FILE *fp = fopen(proc_path, "r");
    if (!fp) {
        return -1;  // 忽略临时退出的进程，报告进程失败
    }

    char state;
    // /proc/[PID]/stat 格式：pid(1) comm(2) state(3) ppid(4) ... utime(14) stime(15)
    // %*d 表示跳过该字段，不存储到变量中
    fscanf(fp, "%d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %llu %llu",
           &pid, comm, &state,  // state 是 char 类型，&state 为 char*，匹配 %c
           utime, stime);       // utime/stime 是 unsigned long long*，匹配 %llu

    fclose(fp);
    return 0;
}



// 参数：proc_list-输出进程列表，max_count-进程列表最大容量
// 返回值：实际获取的进程数量（<0:失败）
static int get_sorted_processes(ProcessInfo *proc_list, int max_count) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        logmessage(CATLOG_ERROR,"opendir /proc failed,请以管理员身份运行");
        return -1;
    }

    struct dirent *entry;
    int proc_count = 0;
    pid_t self_pid = getpid();  // 当前程序PID（避免误杀自己）
    unsigned long long utime1, stime1;//用于第一次的数据记录
    unsigned long long utime2, stime2;//用于第二次的数据记录

    // 首次记录所有进程的初始CPU时间和系统总CPU时间
    unsigned long long initial_total_cpu = get_total_cpu_time();
    if (initial_total_cpu == 0) {
        closedir(dir);
        return -1;
    }

    // 遍历/proc目录，筛选数字命名的PID目录
    while ((entry = readdir(dir)) != NULL && proc_count < max_count) {
        // 检查目录名是否为纯数字（PID特征）
        if (!isdigit(entry->d_name[0])) {
            continue;
        }

        pid_t pid = atoi(entry->d_name);
        // 过滤内核进程（PID=0）和当前程序（避免自杀），将这些放到待杀列表
        if (pid == 0 || pid == self_pid) {
            continue;
        }

        // 记录进程初始CPU时间和进程名
        char comm[256];
        if (get_process_cpu_time(pid, &utime1, &stime1, comm) != 0) {
            continue;
        }

        // 存储初始信息到进程列表
        proc_list[proc_count].pid = pid;
        strncpy(proc_list[proc_count].comm, comm, sizeof(proc_list[proc_count].comm)-1);
        proc_list[proc_count].cpu_usage = 0.0;  // 初始化为0，后续计算

        proc_count++;
    }
    closedir(dir);

    // 等待1秒：让进程有时间产生CPU时间差（避免单次采样误差）
    sleep(1);

    // 第二次读取：计算每个进程的CPU使用率
    unsigned long long final_total_cpu = get_total_cpu_time();
    if (final_total_cpu == 0 || final_total_cpu <= initial_total_cpu) {
        return -1;
    }
    unsigned long long total_cpu_diff = final_total_cpu - initial_total_cpu;  // 系统总CPU时间差

    for (int i = 0; i < proc_count; i++) {
        pid_t pid = proc_list[i].pid;

        char dummy_comm[256];

        // 读取进程最终CPU时间
        if (get_process_cpu_time(pid, &utime2, &stime2, dummy_comm) != 0) {
            proc_list[i].cpu_usage = 0.0;
            continue;
        }

        // 进程CPU时间差 = 最终CPU时间 - 初始CPU时间
        unsigned long long proc_cpu_diff = (utime2 - utime1) + (stime2 - stime1);
        // CPU使用率 = (进程CPU时间差 / 系统总CPU时间差) * 100%（多核系统自动适配，因总CPU时间包含所有核心）
        proc_list[i].cpu_usage = (proc_cpu_diff * 100.0) / total_cpu_diff;
    }

    // 按CPU使用率降序排序
    // 冒泡排序，适合小规模进程列表
    for (int i = 0; i < proc_count-1; i++) {
        for (int j = 0; j < proc_count-i-1; j++) {
            if (proc_list[j].cpu_usage < proc_list[j+1].cpu_usage) {
                ProcessInfo temp = proc_list[j];
                proc_list[j] = proc_list[j+1];
                proc_list[j+1] = temp;
            }
        }
    }

    return proc_count;
}

// 返回值：成功杀死的进程PID（>0）
// 失败返回-1:最大占用进程为自身或者权限不足
pid_t kill_highest_cpu_process(void) {
    // 假设系统最大进程数不超过4096（ARM Linux默认足够，可根据实际调整）
    #define MAX_PROCESSES 4096
    //用于存储进程的pid的列表
    ProcessInfo proc_list[MAX_PROCESSES];

    //获取排序后的进程列表
    int proc_count = get_sorted_processes(proc_list, MAX_PROCESSES);
    if (proc_count <= 0) {
        logmessage(CATLOG_ERROR, "获取进程列表失败或无有效进程\n");
        return -1;
    }

    // 2. 筛选CPU使用率>0的最高占用进程（排除空闲进程）
    pid_t target_pid = -1;
    for (int i = 0; i < proc_count; i++) {
        if (proc_list[i].cpu_usage > 0.0) {
            target_pid = proc_list[i].pid;
            logmessage(CATLOG_INFO,
                "最高CPU占用进程：PID=%d, 进程名=%s, CPU使用率=%.2f%%\n",
                target_pid, proc_list[i].comm, proc_list[i].cpu_usage);
            break;
        }
    }

    if (target_pid == -1) {
        logmessage(CATLOG_ERROR, "未找到有效CPU占用进程，或许最大占用为当前进程或权限获取失败\n");
        return -1;
    }

    //杀死目标进程
    if (kill(target_pid, SIGTERM) != 0) {
        //先尝试SIGTERM优雅终止
        logmessage(CATLOG_ERROR,"kill SIGTERM failed");
        // 尝试强制终止
        if (kill(target_pid, SIGKILL) != 0) {
            logmessage(CATLOG_ERROR,"kill SIGKILL failed");
            return -1;
        }
        logmessage(CATLOG_DEBUG,"已强制终止进程 PID=%d（SIGKILL）\n", target_pid);
    } else {
        // 等待1秒检查进程是否已退出
        sleep(1);
        if (kill(target_pid, 0) == 0) {  // kill(pid,0)不发送信号，仅检查进程是否存在
            if (kill(target_pid, SIGKILL) != 0) {
                perror("kill SIGKILL failed");
                return -1;
            }
            logmessage(CATLOG_DEBUG,"进程 PID=%d 未响应SIGTERM，已强制终止（SIGKILL）\n", target_pid);
        } else {
            logmessage(CATLOG_INFO,"已优雅终止进程 PID=%d（SIGTERM）\n", target_pid);
        }
    }

    return target_pid;
}
