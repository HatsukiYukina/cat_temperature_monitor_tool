#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/cJSON.h"

//函数声明区
char* read_file(const char* filename);  //文件读取
float get_cpu_temperature(const char* path);  //获取路径
int parse_config(const char* config_file, char** temperature_path);

int main(void) {
    char* temperature_path = NULL; //初始化变量
    printf("Hello, World!\n");
    return 0;
}

int temperature_check() {
    if (parse_config("config.json", &temperature_path) != 0) {
        fprintf(stderr, "Failed to parse config file\n");
        return 1;
    }
}