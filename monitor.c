//
// Created by 22723 on 25-8-27.
//
#include "include/monitor.h"
#include "include/configreader.h"
#include "include/catlog.h"
#include "include/temperature.h"
#include "include/csvlogger.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

static MonitorState monitor_state = {0};

//信号处理函数
static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        LOG_INFO("停止监控...\n");
        stop_temperature_monitor();
    }
}

//初始化
static void setup_signal_handlers(void) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

//温度检查
static void check_temperature(const AppConfig* config) {
    float raw_temp = read_cpu_temperature(config);

    if (raw_temp < 0) {
        LOG_ERROR("温度读取失败\n");
        return;
    }

    float real_temp = calculate_real_temperature(raw_temp, config->multiple);

    LOG_DEBUG("温度: %.2f°C\n", real_temp);

    if (is_csv_enabled()) {
        const char* status;
        if (real_temp < 40.0f) {
            status = "NORMAL";
        } else if (real_temp < 60.0f) {
            status = "WARM";
        } else if (real_temp < 80.0f) {
            status = "HOT";
        } else {
            status = "CRITICAL";
        }

        log_temperature_to_csv(real_temp, status);
    }

    //温度检查
    if (real_temp < 40.0f) {
        //可以在这里添加条件和执行代码，下同
    }
    else if (real_temp < 60.0f) {
    }
    else if (real_temp < 80.0f) {
        monitor_state.alert_count++;
    }
    else {
        monitor_state.alert_count++;

        //如果超过最大温度限制
        if (real_temp > monitor_state.max_temperature) {
            LOG_ERROR("温度超过安全限制 (%d°C)\n", monitor_state.max_temperature);
        }
    }

    //定期报告统计信息，底下那个数能改
    if (monitor_state.alert_count > 0 && monitor_state.alert_count % 10 == 0) {
        LOG_WARN("已记录 %d 次温度警报\n", monitor_state.alert_count);
    }
}

//开始温度监控
void start_temperature_monitor(AppConfig* config) {
    if (!config) {
        LOG_ERROR("无效的配置，无法启动监控\n");
        return;
    }

    if (monitor_state.running) {
        LOG_WARN("监控已经在运行中\n");
        return;
    }

    LOG_INFO("检查间隔: %d秒\n", config->check_interval);
    LOG_INFO("最大温度限制: %d°C\n", monitor_state.max_temperature);
    LOG_INFO("按 Ctrl+C 停止监控\n");
    LOG_INFO("========================================\n");

    //初始化
    monitor_state.running = 1;
    monitor_state.interval = config->check_interval;
    monitor_state.alert_count = 0;
    monitor_state.max_temperature = 85;  // 默认85°C

    int init_csv_file(const char* );
    //设置信号处理
    setup_signal_handlers();

    int cycle_count = 0;

    //监控循环
    while (monitor_state.running) {
        cycle_count++;

        LOG_DEBUG("第 %d 次温度检查...\n", cycle_count);
        check_temperature(config);

        //等待间隔
        int remaining = monitor_state.interval;
        while (remaining > 0 && monitor_state.running) {
            sleep(1);
            remaining--;

            if (!monitor_state.running) {
                break;
            }
        }

        //统计信息,50下出来一次，可改
        if (cycle_count % 50 == 0) {
            LOG_INFO("已运行 %d 次检查, %d 次警报\n",
                    cycle_count, monitor_state.alert_count);
        }
    }

    LOG_INFO("监控已停止\n");
    LOG_INFO("总共运行: %d 次检查\n", cycle_count);
    LOG_INFO("温度警报: %d 次\n", monitor_state.alert_count);
}

// 停止温度监控
void stop_temperature_monitor(void) {
    if (monitor_state.running) {
        monitor_state.running = 0;
        LOG_INFO("停止监控\n");
    }
}

//检查监控是否正在运行
int is_monitor_running(void) {
    return monitor_state.running;
}

//监控间隔
void set_monitor_interval(int interval) {
    if (interval > 0) {
        monitor_state.interval = interval;
        LOG_INFO("监控间隔设置为: %d秒", interval);
    }
}

//最大温度限制
void set_max_temperature_limit(int max_temp) {
    if (max_temp > 0) {
        monitor_state.max_temperature = max_temp;
        LOG_INFO("最大温度限制设置为: %d°C", max_temp);
    }
}