//
// Created by 22723 on 25-8-27.
//

#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "configreader.h"

//温度函数
float read_cpu_temperature(const AppConfig* config);
float calculate_real_temperature(float raw_value, float multiple);
void print_temperature_info(float temperature);

#endif //TEMPERATURE_H
