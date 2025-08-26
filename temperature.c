//
// Created by 22723 on 25-8-27.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "include/configreader.h"
#include "include/catlog.h"


//读取CPU温度原始值
float read_cpu_temperature(const AppConfig* config) {
    if (!config || !config->temperature_path) {
        logmessage(3, "无效的配置\n");
        return -1.0f;
    }

    FILE* file = fopen(config->temperature_path, "r");
    if (!file) {
        logmessage(3, "无法打开温度文件:%s\n", config->temperature_path);
        logmessage(3, "错误信息:%s\n", strerror(errno));
        return -1.0f;
    }

    int raw_value;
    if (fscanf(file, "%d", &raw_value) != 1) {
        logmessage(3, "读取温度值失败\n");
        fclose(file);
        return -1.0f;
    }

    fclose(file);

    logmessage(0, "原始温度值: %d\n", raw_value);
    return (float)raw_value;
}

//计算温度
float calculate_real_temperature(float raw_value, float multiple) {
    if (raw_value < 0) {
        logmessage(3, "无效的原始温度值:%.2f\n", raw_value);
        return -1.0f;
    }

    float real_temperature = raw_value * multiple;
    logmessage(0, "计算后的温度:%.2f = %.0f * %.6f\n",
              real_temperature, raw_value, multiple);

    return real_temperature;
}

//输出温度信息
void print_temperature_info(float temperature) {
    if (temperature < 0) {
        logmessage(3, "温度读取失败\n");
        return;
    }

    logmessage(1, "当前CPU温度:%.2f°C\n", temperature);

    if (temperature < 40.0f) {
        logmessage(1, "温度凉爽\n");
    }
    else if (temperature < 60.0f) {
        logmessage(1, "温度正常\n");
    }
    else if (temperature < 80.0f) {
        logmessage(2, "温度偏高\n");
    }
    else {
        logmessage(2, "温度过高\n");
    }
}