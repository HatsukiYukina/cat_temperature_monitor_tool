//
// Created by 22723 on 25-8-27.
//

#ifndef MONITOR_H
#define MONITOR_H
#include "configreader.h"
#include <unistd.h>

typedef struct {
    int running;//是否正在运行
    int interval;//检查间隔
    int max_temperature;//最高温度限制
    int alert_count;// 警报计数

    int killer_enable; //是否启用killer
    int alert_burn_count;// 超出安全上限一杀后的过温警报次数
    int alert_burn_count_max;// 达到二次杀进程所需的超出安全上限的警报次数
    int reboot_max_temperature; //触发这个温度直接重启系统
    int kill_alert_count;//在超出温度后的小延时所需的计数次数（避免因为一次瞬时温度杀）
    int kill_retry_count; //杀单个进程最高重试次数

} MonitorState;

//函数声明
void start_temperature_monitor(AppConfig* config);
void stop_temperature_monitor(void);
int is_monitor_running(void);
void set_monitor_interval(int interval);
void set_max_temperature_limit(int max_temp);

#endif //MONITOR_H
