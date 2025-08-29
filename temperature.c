//
// Created by 22723 on 25-8-29.
//
// temperature_reader.c
#include "include/configreader.h"
#include "include/temperature.h"
#include <stdio.h>
#include <stdlib.h>

//读取原始值
float read_cpu_temperature(const AppConfig* config) {
    if (!config || !config->temperature_path) {
        return -1.0f;
    }

    FILE* thermal_file = fopen(config->temperature_path, "r");
    if (!thermal_file) {
        return -1.0f;
    }

    int temp_raw = 0;
    if (fscanf(thermal_file, "%d", &temp_raw) != 1) {
        fclose(thermal_file);
        return -1.0f;
    }
    fclose(thermal_file);

    return (float)temp_raw;
}

//根据倍率计算实际温度
float calculate_real_temperature(float raw_temperature, float multiple) {
    return raw_temperature * multiple;
}