//
// Created by 22723 on 25-8-27.
//

#ifndef CSVLOGGER_H
#define CSVLOGGER_H
#include "configreader.h"

// CSV记录函数
void init_csv_logger(const AppConfig* config);
void log_temperature_to_csv(float temperature, const char* status);
void close_csv_logger(void);
int is_csv_logging_enabled(void);

#endif //CSVLOGGER_H
