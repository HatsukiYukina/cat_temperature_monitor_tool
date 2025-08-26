//
// Created by 22723 on 25-8-26.
// 可以在windows和linux下使用
// MIT License
// Copyright © 2025 HatsukiYukina
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
#include "include/catlog.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//只显示info以上，可配置
static int current_log_level = CATLOG_INFO;

static FILE* log_file = NULL;
static char log_filename[256] = {0};
static int file_logging_enabled = 0;

//获取当前时间
static void get_current_time(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}
//获取日志等级对应的名称
const char* get_log_level_name(int level) {
    switch (level) {
        case CATLOG_DEBUG: return "DEBUG";
        case CATLOG_INFO:  return "INFO";
        case CATLOG_WARN:  return "WARN";
        case CATLOG_ERROR: return "ERROR";
        case CATLOG_FATAL: return "FATAL";
        default:           return "UNKNOWN";
    }
}


// 设置日志等级
void set_log_level(int level) {
    current_log_level = level;
}

// 获取当前日志等级
int get_log_level(void) {
    return current_log_level;
}

//启用文件日志
void enable_file_logging(const char* filename) {
    if (filename) {
        set_log_file(filename);
    }
    file_logging_enabled = 1;
}

//禁用文件日志
void disable_file_logging(void) {
    file_logging_enabled = 0;
}

//设置日志文件
void set_log_file(const char* filename) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }

    if (filename && strlen(filename) > 0) {
        strncpy(log_filename, filename, sizeof(log_filename) - 1);
        log_file = fopen(filename, "a"); // 追加模式
        if (!log_file) {
            fprintf(stderr, "开启logfile失败: %s\n", filename);
            log_filename[0] = '\0';
        }
    }
}

//检查文件日志是否启用
int is_file_logging_enabled(void) {
    return file_logging_enabled;
}

//输出到文件
static void log_to_file(const char* message) {
    if (file_logging_enabled && log_file) {
        fprintf(log_file, "%s\n", message);
        fflush(log_file); //立即写入文件
    }
}

//带颜色输出的日志函数
void logmessage(int level, const char *format, ...) {
    if (level < current_log_level) {
        return;
    }

    //获取当前时间到变量
    char time_buffer[20];
    get_current_time(time_buffer, sizeof(time_buffer));

    //设置颜色(ANSI转义
    const char* color_code = "";
    const char* reset_code = "";

    //如果是彩色终端，输出彩色的log
    if (isatty(fileno(stdout))) {
        reset_code = "\033[0m";
        switch (level) {
            case CATLOG_DEBUG: color_code = "\033[36m"; //青色
                break;
            case CATLOG_INFO:  color_code = "\033[32m"; //绿色
                break;
            case CATLOG_WARN:  color_code = "\033[33m"; //黄色
                break;
            case CATLOG_ERROR: color_code = "\033[31m"; //红色
                break;
            case CATLOG_FATAL: color_code = "\033[35m"; //给靓仔看的红色
                break;
            default:           color_code = "\033[0m";  //默认
        }
    }
    //格式化消息
    char message_buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message_buffer, sizeof(message_buffer), format, args);
    va_end(args);

    //拼接输出日志前缀，时间，等级等


    //输出到终端的
    char terminal_line[1024];
    snprintf(terminal_line, sizeof(terminal_line), "%s[%s | %-5s]->%s:%s",
             color_code, time_buffer, get_log_level_name(level), reset_code, message_buffer);

    //输出到终端）
    fprintf(stdout, "%s", terminal_line); //这里你需要的话可以加尾缀
    fflush(stdout);
    //准备没有颜色的输出到文件
    char file_line[1024];
    snprintf(file_line, sizeof(file_line), "[%s | %-5s]->:%s",
             time_buffer, get_log_level_name(level), message_buffer);

    log_to_file(file_line);

    //byd你吃到FATAL了你知道吗？
    if (level == CATLOG_FATAL) {
        if (log_file) {
            fclose(log_file);
            log_file = NULL;
        }
        exit(EXIT_FAILURE);
    }
}

void __attribute__((destructor)) cleanup_logging(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}