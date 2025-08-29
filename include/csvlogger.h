//
// Created by 22723 on 25-8-27.
//

#ifndef CSVLOGGER_H
#define CSVLOGGER_H
// CSV记录结构体
typedef struct {
    long timestamp;
    char datetime[20];
    float temperature;
    char status[10];
} CSVRecord;

//初始化
int init_csv_file(const char* filename);
//关闭文件
void close_csv_file(void);
//记录到CSV
void log_temperature_to_csv(float temperature, const char* status);
//是否启用
int is_csv_enabled(void);

//读取
int get_csv_line_count(void);
int read_csv_line(int line_number, CSVRecord* record);
int read_last_csv_line(CSVRecord* record);
int read_csv_range(int start_line, int num_lines, CSVRecord* records);

//统计
float get_average_temperature(int last_n_lines);
float get_max_temperature(int last_n_lines);
float get_min_temperature(int last_n_lines);
int get_status_count(const char* status, int last_n_lines);

//工具
void print_csv_stats(void);
void print_recent_csv_lines(int num_lines);

#endif //CSVLOGGER_H
