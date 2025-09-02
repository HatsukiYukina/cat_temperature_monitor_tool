//
// Created by CAT on 25-8-26.
// MIT License
// Copyright © 2025 HatsukiYukina
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
#include "include/configreader.h"
#include <math.h>
#include "include/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/catlog.h"

//从文件读取json
static char* read_file_content(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        LOG_ERROR("无法打开配置文件:%s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(length + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

//从JSON获取字符串值
static char* get_json_string(cJSON* json, const char* key, const char* default_value) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(item) && item->valuestring != NULL) {
        return strdup(item->valuestring);
    }
    return default_value ? strdup(default_value) : NULL;
}

//从JSON获取数字值
static float get_json_number(cJSON* json, const char* key, float default_value) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(item)) {
        return (float)item->valuedouble;
    }
    return default_value;
}

//从JSON获取整数值
static int get_json_int(cJSON* json, const char* key, int default_value) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(item)) {
        return item->valueint;
    }
    return default_value;
}

//读配置
AppConfig* read_app_config(const char* config_file) {
    //读文件内容
    char* json_data = read_file_content(config_file);
    if (!json_data) {
        return NULL;
    }

    //解析
    cJSON* json = cJSON_Parse(json_data);
    free(json_data);

    if (!json) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            LOG_ERROR("Json解析错误:%s\n", error_ptr);
        }
        return NULL;
    }

    //分配配置的结构体
    AppConfig* config = malloc(sizeof(AppConfig));
    if (!config) {
        LOG_ERROR("内存分配失败\n");
        cJSON_Delete(json);
        return NULL;
    }

    //初始化硬编码默认数值
    config->temperature_path = NULL;
    config->multiple = 0.001f;
    config->check_interval = 5;
    config->logcsv_enable = 1;
    config->log_file = "latest.log";
    config->csv_file = "/var/log/ctmt/temp.csv";
    config->httpsend_enable = 0;
    config->http_url = "http://localhost:8080";
    config->access_token = NULL;


    //解析temperature
    cJSON* temperature = cJSON_GetObjectItemCaseSensitive(json, "temperature");
    if (cJSON_IsObject(temperature)) {
        config->temperature_path = get_json_string(temperature, "path", NULL);
        config->multiple = get_json_number(temperature, "multiple", 0.001f);
    }

    //解析settings
    cJSON* settings = cJSON_GetObjectItemCaseSensitive(json, "settings");
    if (cJSON_IsObject(settings)) {
        config->check_interval = get_json_int(settings, "check_interval", 5);
        config->logcsv_enable = get_json_int(settings, "logcsv_enable", 1);
        config->log_file = get_json_string(settings, "log_file", NULL);
        config->csv_file = get_json_string(settings, "csv_file", NULL);
    }
    //解析onebothttp设置
    cJSON* onebot_http = cJSON_GetObjectItemCaseSensitive(json, "onebot_http");
    if (cJSON_IsObject(onebot_http)) {
        config->httpsend_enable = get_json_int(onebot_http, "httpsend_enable", 1);
        config->http_url = get_json_string(onebot_http, "http_url", NULL);
        config->access_token = get_json_string(onebot_http, "access_token", NULL);
    }

    cJSON_Delete(json);

    //验证必要配置
    if (!config->temperature_path) {
        LOG_ERROR("配置文件中未找到温度路径\n");
        free_app_config(config);
        return NULL;
    }

    LOG_INFO("Json配置文件读取成功\n");
    return config;
}

//释放配置
void free_app_config(AppConfig* config) {
    if (config) {
        free(config->temperature_path);
        free(config->log_file);
        free(config->csv_file);
        free(config);
    }
}

//输出信息
void print_app_config(const AppConfig* config) {
    if (!config) {
        LOG_ERROR("配置为空\n");
        return;
    }

    LOG_INFO("温度监控->系统温度文件路径: %s\n", config->temperature_path);
    LOG_INFO("温度监控->转换倍率: %.6f\n", config->multiple);
    LOG_INFO("温度监控->温度检查间隔: %ds\n", config->check_interval);
    LOG_INFO("温度监控->程序日志文件: %s\n", config->log_file);
    LOG_INFO("温度监控->温度日志文件: %s\n", config->csv_file);
}
