//
// Created by 22723 on 25-8-26.
// 可以在windows和linux下使用
// MIT License
// Copyright © 2025 HatsukiYukina
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef CATLOG_H
#define CATLOG_H
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <time.h>
//#include <unistd.h>

//日志等级
enum {
    CATLOG_DEBUG = 0,
    CATLOG_INFO  = 1,
    CATLOG_WARN  = 2,
    CATLOG_ERROR = 3,
    CATLOG_FATAL = 4
};

//函数声明
void logmessage(int level, const char* format, ...);
void set_log_level(int level);
int get_log_level(void);
const char* get_log_level_name(int level);

void enable_file_logging(const char* filename);
void disable_file_logging(void);
void set_log_file(const char* filename);
int is_file_logging_enabled(void);

//宏定义
#define LOG_DEBUG(...) logmessage(CATLOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  logmessage(CATLOG_INFO, __VA_ARGS__)
#define LOG_WARN(...)  logmessage(CATLOG_WARN, __VA_ARGS__)
#define LOG_ERROR(...) logmessage(CATLOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) logmessage(CATLOG_FATAL, __VA_ARGS__)

#endif //CATLOG_H
