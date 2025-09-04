//
// Created by 22723 on 25-8-27.
//

#ifndef MONITOR_H
#define MONITOR_H
#include "configreader.h"

typedef struct {
    int running;//是否正在运行
    int interval;//检查间隔
    int max_temperature;//最高温度限制
    int alert_count;// 警报计数
    int alert_burn_count;// 超出安全上限后的警报次数
    int alert_burn_count_max;// 进行二次杀进程所需的超出安全上限的警报次数

} MonitorState;

//函数声明
void start_temperature_monitor(AppConfig* config);
void stop_temperature_monitor(void);
int is_monitor_running(void);
void set_monitor_interval(int interval);
void set_max_temperature_limit(int max_temp);

#endif //MONITOR_H
