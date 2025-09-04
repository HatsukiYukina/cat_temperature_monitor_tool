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
    //temperature object
    char* temperature_path;  //温度文件路径
    float multiple;          //倍率
    float normal_temp; //正常温度检测
    float warm_temp; //有点暖和
    float hot_temp;  //红温了
    //settings object
    int check_interval;      //检查间隔（秒）
    int logcsv_enable;         //是否启用csv日志
    char* log_file;          //日志文件路径
    char* csv_file;         //csv记录表路径
    //onebothttp object
    int httpsend_enable;    //启用onebot http
    char* http_url;     //http址
    char* access_token;      //访问令牌
    //killer object
    int killer_enable; //是否启用killer
    int max_temperature;  //最大温度
    int alert_burn_count_max; //进行二次杀进程所需的警报次数
    int reboot_max_temperature; //触发这个温度直接重启系统
    int kill_alert_count; //连续警报几次杀
    int kill_retry_count; //杀进程重试次数
    char* host_name; //报告时使用的名称
} AppConfig;


//函数声明
AppConfig* read_app_config(const char* config_file);
void free_app_config(AppConfig* config);
void print_app_config(const AppConfig* config);

//float read_cpu_temperature(const AppConfig* config);
//float calculate_real_temperature(float raw_value, float multiple);
//我警告你，注释掉的代码肯定有原因，不要随便动
void print_temperature_info(float temperature);

#endif //CONFIGREADER_H
