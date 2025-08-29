//
// Created by 22723 on 25-8-27.
//

#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "configreader.h"

float read_cpu_temperature(const AppConfig* config);
float calculate_real_temperature(float raw_temperature, float multiple);

#endif //TEMPERATURE_H
