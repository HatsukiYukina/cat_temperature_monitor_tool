//
// MIT License
// Copyright © 2025 HatsukiYukina
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//注意：本项目使用的非内置头文件全部放在/include下
#include <unistd.h>
#include "include/cJSON.h"
#include "include/configreader.h"
#include "include/catlog.h"
#include "include/cat_httpsender.h"
#include "include/monitor.h"
#include "include/csvlogger.h"


int main(void) {
    enable_file_logging("latest.log"); //启动并指定文件名
    logmessage(1,"将读取/etc/ctmt/config.json\n");
    AppConfig* config = read_app_config("/etc/ctmt/config.json"); //我是硬编码仙人
    HTTPSender* http_sender = http_sender_init(config);
    //config大检查
    if (!config) {
        fprintf(stderr, "\n");
        logmessage(3,"无法读取config\n");
        return 1;
    }
    set_log_level(1);
    //输出一遍参数
    print_app_config(config);
    //初始化csv
    if (config->logcsv_enable && config->csv_file) {  // 改为使用 config->csv_file
        if (init_csv_file(config->csv_file)) {
            LOG_INFO("温度日志文件: %s\n", config->csv_file);
        }
    }


    //启动监控循环
    start_temperature_monitor(config);
    //输出统计数据
    print_csv_stats();
    CSVRecord specific_record;
    if (read_csv_line(114, &specific_record)) {  // 读取第5行
        LOG_INFO("第114条记录: %s - %.2f°C\n",//?喵喵喵
                 specific_record.datetime,
                 specific_record.temperature);
    }
    //输出最后记录
    CSVRecord last_record;
    if (read_last_csv_line(&last_record)) {
        LOG_INFO("最后记录: %s - %.2f°C (%s)\n",
                 last_record.datetime,
                 last_record.temperature,
                 last_record.status);
    }
    //释放配置文件和csv文件
    close_csv_file();
    free_app_config(config);

    return 0;
}