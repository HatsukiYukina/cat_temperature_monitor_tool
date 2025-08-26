//
// Created by CAT on 25-8-26.
// MIT License
// Copyright © 2025 HatsukiYukina
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//

#ifndef CONFIGREADER_H
#define CONFIGREADER_H
//变量加在这
typedef struct {
    char* temperature_path;  //温度文件路径
    float multiple;          //倍率
    int check_interval;      //检查间隔（秒）
    int logcsv_enable;         //是否启用csv日志
    char* log_file;          //日志文件路径
} AppConfig;


//函数声明
AppConfig* read_app_config(const char* config_file);
void free_app_config(AppConfig* config);
void print_app_config(const AppConfig* config);

float read_cpu_temperature(const AppConfig* config);
float calculate_real_temperature(float raw_value, float multiple);
void print_temperature_info(float temperature);

#endif //CONFIGREADER_H
