//
// Created by 22723 on 25-8-27.
//
#include "include/csvlogger.h"
#include "include/configreader.h"
#include "include/catlog.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

static FILE* csv_file = NULL;
static int csv_enabled = 0;

//初始化CSV记录器
void init_csv_logger(const AppConfig* config) {
    if (!config) {
        return;
    }

    //CSV文件路径
    const char* default_csv_path = "temperature_log.csv";

    //打开CSV文件
    csv_file = fopen(default_csv_path, "a");
    if (!csv_file) {
        LOG_ERROR("无法创建CSV日志文件: %s", default_csv_path);
        csv_enabled = 0;
        return;
    }

    //是新文件的话写入CSV表头
    fseek(csv_file, 0, SEEK_END);
    if (ftell(csv_file) == 0) {
        fprintf(csv_file, "timestamp,datetime,temperature_c,status\n");
        fflush(csv_file);
    }

    csv_enabled = 1;
    LOG_INFO("CSV日志已启用: %s", default_csv_path);
}

//记录温度到CSV
void log_temperature_to_csv(float temperature, const char* status) {
    if (!csv_enabled || !csv_file) {
        return;
    }

    //获取时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    //格式化
    char datetime_buffer[20];
    strftime(datetime_buffer, sizeof(datetime_buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    //写入CSV行
    fprintf(csv_file, "%ld,%s,%.2f,%s\n",
            now, datetime_buffer, temperature, status);
    fflush(csv_file); //立即写入
}

//关闭记录器
void close_csv_logger(void) {
    if (csv_file) {
        fclose(csv_file);
        csv_file = NULL;
    }
    csv_enabled = 0;
    LOG_INFO("CSV日志已关闭");
}

//检查CSV记录是否启用
int is_csv_logging_enabled(void) {
    return csv_enabled;
}