#include "include/csvlogger.h"
#include "include/catlog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

static FILE* csv_file = NULL;
static int csv_enabled = 0;
static char csv_filename[256] = {0};

//初始化CSV
int init_csv_file(const char* filename) {
    if (!filename) {
        LOG_ERROR("CSV文件名不能为空\n");
        return 0;
    }

    char dir_path[256];
    strncpy(dir_path, filename, sizeof(dir_path));
    char* last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        // 创建目录（如果不存在）
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", dir_path);
        system(cmd);
    }

    strncpy(csv_filename, filename, sizeof(csv_filename) - 1);

    //打开CSV文件
    csv_file = fopen(filename, "a");
    if (!csv_file) {
        LOG_ERROR("无法创建CSV文件: %s\n", filename);
        csv_enabled = 0;
        return 0;
    }

    //给新文件写入CSV表头
    fseek(csv_file, 0, SEEK_END);
    if (ftell(csv_file) == 0) {
        fprintf(csv_file, "timestamp,datetime,temperature,status\n");
        fflush(csv_file);
    }

    csv_enabled = 1;
    LOG_DEBUG("CSV文件已初始化: %s\n", filename);
    return 1;
}

//关闭文件
void close_csv_file(void) {
    if (csv_file) {
        fclose(csv_file);
        csv_file = NULL;
    }
    csv_enabled = 0;
    LOG_INFO("CSV文件已关闭\n");
}

//记录到CSV
void log_temperature_to_csv(float temperature, const char* status) {
    if (!csv_enabled || !csv_file) {
        return;
    }

    //获取时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char datetime_buffer[20];
    strftime(datetime_buffer, sizeof(datetime_buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    //写入CSV行
    fprintf(csv_file, "%ld,%s,%.2f,%s\n",
            now, datetime_buffer, temperature, status);
    fflush(csv_file); // 确保立即写入文件
}

//检查CSV是否启用
int is_csv_enabled(void) {
    return csv_enabled;
}

//获取CSV文件行数
int get_csv_line_count(void) {
    if (!csv_filename[0]) {
        return 0;
    }

    FILE* file = fopen(csv_filename, "r");
    if (!file) {
        return 0;
    }

    int line_count = 0;
    char buffer[256];

    // 跳过表头
    fgets(buffer, sizeof(buffer), file);

    while (fgets(buffer, sizeof(buffer), file)) {
        line_count++;
    }

    fclose(file);
    return line_count;
}

//读取指定行的CSV数据
int read_csv_line(int line_number, CSVRecord* record) {
    if (line_number <= 0 || !record || !csv_filename[0]) {
        return 0;
    }

    FILE* file = fopen(csv_filename, "r");
    if (!file) {
        return 0;
    }

    char buffer[256];
    int current_line = 0;

    //跳过表头
    fgets(buffer, sizeof(buffer), file);

    while (fgets(buffer, sizeof(buffer), file)) {
        current_line++;

        if (current_line == line_number) {
            // 解析CSV行
            if (sscanf(buffer, "%ld,%19[^,],%f,%9[^,\n]",
                      &record->timestamp,
                      record->datetime,
                      &record->temperature,
                      record->status) == 4) {
                fclose(file);
                return 1;
            }
            break;
        }
    }

    fclose(file);
    return 0;
}

//读取最后一行
int read_last_csv_line(CSVRecord* record) {
    int total_lines = get_csv_line_count();
    if (total_lines > 0) {
        return read_csv_line(total_lines, record);
    }
    return 0;
}

//读取CSV数据范围
int read_csv_range(int start_line, int num_lines, CSVRecord* records) {
    if (start_line <= 0 || num_lines <= 0 || !records || !csv_filename[0]) {
        return 0;
    }

    FILE* file = fopen(csv_filename, "r");
    if (!file) {
        return 0;
    }

    char buffer[256];
    int current_line = 0;
    int records_read = 0;

    //跳过表头
    fgets(buffer, sizeof(buffer), file);

    while (fgets(buffer, sizeof(buffer), file) && records_read < num_lines) {
        current_line++;

        if (current_line >= start_line) {
            if (sscanf(buffer, "%ld,%19[^,],%f,%9[^,\n]",
                      &records[records_read].timestamp,
                      records[records_read].datetime,
                      &records[records_read].temperature,
                      records[records_read].status) == 4) {
                records_read++;
            }
        }

        //如果已经超过需要的范围。退出
        if (current_line >= start_line + num_lines - 1) {
            break;
        }
    }

    fclose(file);
    return records_read;
}

//获取平均温度
float get_average_temperature(int last_n_lines) {
    if (last_n_lines <= 0) {
        return 0.0f;
    }

    int total_lines = get_csv_line_count();
    int start_line = total_lines - last_n_lines + 1;
    if (start_line < 1) start_line = 1;

    CSVRecord* records = malloc(last_n_lines * sizeof(CSVRecord));
    if (!records) {
        return 0.0f;
    }

    int count = read_csv_range(start_line, last_n_lines, records);
    float sum = 0.0f;

    for (int i = 0; i < count; i++) {
        sum += records[i].temperature;
    }

    free(records);
    return count > 0 ? sum / count : 0.0f;
}

//获取最高温度
float get_max_temperature(int last_n_lines) {
    if (last_n_lines <= 0) {
        return 0.0f;
    }

    int total_lines = get_csv_line_count();
    int start_line = total_lines - last_n_lines + 1;
    if (start_line < 1) start_line = 1;

    CSVRecord* records = malloc(last_n_lines * sizeof(CSVRecord));
    if (!records) {
        return 0.0f;
    }

    int count = read_csv_range(start_line, last_n_lines, records);
    float max_temp = -1000.0f;

    for (int i = 0; i < count; i++) {
        if (records[i].temperature > max_temp) {
            max_temp = records[i].temperature;
        }
    }

    free(records);
    return max_temp > -1000.0f ? max_temp : 0.0f;
}

//获取最低温度
float get_min_temperature(int last_n_lines) {
    if (last_n_lines <= 0) {
        return 0.0f;
    }

    int total_lines = get_csv_line_count();
    int start_line = total_lines - last_n_lines + 1;
    if (start_line < 1) start_line = 1;

    CSVRecord* records = malloc(last_n_lines * sizeof(CSVRecord));
    if (!records) {
        return 0.0f;
    }

    int count = read_csv_range(start_line, last_n_lines, records);
    float min_temp = 1000.0f;

    for (int i = 0; i < count; i++) {
        if (records[i].temperature < min_temp) {
            min_temp = records[i].temperature;
        }
    }

    free(records);
    return min_temp < 1000.0f ? min_temp : 0.0f;
}

//获取状态计数
int get_status_count(const char* status, int last_n_lines) {
    if (!status || last_n_lines <= 0) {
        return 0;
    }

    int total_lines = get_csv_line_count();
    int start_line = total_lines - last_n_lines + 1;
    if (start_line < 1) start_line = 1;

    CSVRecord* records = malloc(last_n_lines * sizeof(CSVRecord));
    if (!records) {
        return 0;
    }

    int count = read_csv_range(start_line, last_n_lines, records);
    int status_count = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].status, status) == 0) {
            status_count++;
        }
    }

    free(records);
    return status_count;
}

//打印统计信息
void print_csv_stats(void) {
    int total_lines = get_csv_line_count();
    if (total_lines == 0) {
        LOG_INFO("CSV文件中没有数据\n");
        return;
    }

    LOG_INFO("总记录数: %d\n", total_lines);
    LOG_INFO("平均温度: %.2f°C\n", get_average_temperature(total_lines));
    LOG_INFO("最高温度: %.2f°C\n", get_max_temperature(total_lines));
    LOG_INFO("最低温度: %.2f°C\n", get_min_temperature(total_lines));

    const char* statuses[] = {"COOL", "NORMAL", "WARM", "HOT", NULL};
    for (int i = 0; statuses[i] != NULL; i++) {
        int count = get_status_count(statuses[i], total_lines);
        if (count > 0) {
            LOG_INFO("%s状态: %d次\n", statuses[i], count);
        }
    }
}

//打印最近的CSV行
void print_recent_csv_lines(int num_lines) {
    int total_lines = get_csv_line_count();
    if (total_lines == 0) {
        LOG_INFO("CSV文件中没有数据\n");
        return;
    }

    int start_line = total_lines - num_lines + 1;
    if (start_line < 1) start_line = 1;

    CSVRecord* records = malloc(num_lines * sizeof(CSVRecord));
    if (!records) {
        return;
    }

    int count = read_csv_range(start_line, num_lines, records);

    LOG_INFO("最近 %d 条记录\n", count);
    for (int i = 0; i < count; i++) {
        LOG_INFO("[%d] %s - %.2f°C (%s)\n",
                start_line + i,
                records[i].datetime,
                records[i].temperature,
                records[i].status);
    }

    free(records);
}