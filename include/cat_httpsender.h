//
// Created by 22723 on 25-8-31.
//

#ifndef ONEBOT_H
#define ONEBOT_H

#include "configreader.h"
#include <stdbool.h>
#include <stdint.h>

//发送器上下文
typedef struct HTTPSender HTTPSender;
//初始化发送器
HTTPSender* http_sender_init(AppConfig* config);
//销毁发送器
void http_sender_destroy(HTTPSender* sender);
//发送私聊消息
bool http_send_private_msg(HTTPSender* sender, int64_t user_id, const char* message);
//发送群消息
bool http_send_group_msg(HTTPSender* sender, int64_t group_id, const char* message);
//自定义请求
bool http_send_request(HTTPSender* sender, const char* api_path, const char* params);

//我认为httpsendonly已经可以支持这个工具的使用目标，常用的发送私聊与群聊消息接口已封装
// http_send_private_msg(http_sender, 2272353474, "消息发送自catonebot框架！");
// 用上面的样式就可以发送消息
//自定义请求见onebot标准
//https://283375.github.io/onebot_v11_vitepress/communication/http.html


#endif //ONEBOT_H
