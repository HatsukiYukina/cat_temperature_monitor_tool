#include "include/cat_httpsender.h"
#include "include/catlog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

//发送器结构
struct HTTPSender {
    char http_url[256];      //基础URL
    char access_token[128];  //令牌
    bool enabled;           //是否启用
};

//初始化发送器
HTTPSender* http_sender_init(AppConfig* config) {
    if (!config || !config->http_url) {
        LOG_WARN("HTTP发送器配置不完整\n");
        return NULL;
    }
    //初始化libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    HTTPSender* sender = malloc(sizeof(HTTPSender));
    if (!sender) {
        LOG_ERROR("内存分配失败\n");
        return NULL;
    }

    sender->enabled = config->httpsend_enable;
    strncpy(sender->http_url, config->http_url, sizeof(sender->http_url) - 1);

    //设置访问令牌
    if (config->access_token) {
        strncpy(sender->access_token, config->access_token, sizeof(sender->access_token) - 1);
    } else {
        sender->access_token[0] = '\0';
    }

    //LOG_INFO("输入http地址: %s", sender->http_url);
    return sender;
}

//销毁发送器
void http_sender_destroy(HTTPSender* sender) {
    if (!sender) return;

    free(sender);
    curl_global_cleanup();

    LOG_INFO("HTTP发送器已销毁");
}

//响应回调函数
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

//发送Http请求
static bool send_http_request(const char* url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("初始化CURL失败");
        return false;
    }

    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);  // 5秒超时

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

    res = curl_easy_perform(curl);

    bool success = (res == CURLE_OK);
    if (!success) {
        LOG_ERROR("HTTP请求失败: %s", curl_easy_strerror(res));
    } else {
        LOG_DEBUG("HTTP请求成功: %s", url);
    }

    curl_easy_cleanup(curl);
    return success;
}

//URL编码函数
static char* url_encode(const char* str) {
    CURL* curl = curl_easy_init();
    if (!curl) return NULL;

    char* encoded = curl_easy_escape(curl, str, 0);
    curl_easy_cleanup(curl);

    return encoded;
}

//发送私聊消息
bool http_send_private_msg(HTTPSender* sender, int64_t user_id, const char* message) {
    if (!sender || !sender->enabled || !message) {
        return false;
    }

    //URL编码消息内容
    char* encoded_msg = url_encode(message);
    if (!encoded_msg) {
        LOG_ERROR("消息编码失败");
        return false;
    }

    //拼好URL
    char url[512];
    if (strlen(sender->access_token) > 0) {
        snprintf(url, sizeof(url), "%s/send_private_msg?access_token=%s&user_id=%lld&message=%s",
                sender->http_url, sender->access_token, user_id, encoded_msg);
    } else {
        snprintf(url, sizeof(url), "%s/send_private_msg?user_id=%lld&message=%s",
                sender->http_url, user_id, encoded_msg);
    }

    LOG_DEBUG("发送私聊消息: %s", url);

    bool result = send_http_request(url);
    curl_free(encoded_msg);

    return result;
}

// 发送群消息
bool http_send_group_msg(HTTPSender* sender, int64_t group_id, const char* message) {
    if (!sender || !sender->enabled || !message) {
        return false;
    }

    char* encoded_msg = url_encode(message);
    if (!encoded_msg) {
        LOG_ERROR("消息编码失败");
        return false;
    }

    char url[512];
    if (strlen(sender->access_token) > 0) {
        snprintf(url, sizeof(url), "%s/send_group_msg?access_token=%s&group_id=%lld&message=%s",
                sender->http_url, sender->access_token, group_id, encoded_msg);
    } else {
        snprintf(url, sizeof(url), "%s/send_group_msg?group_id=%lld&message=%s",
                sender->http_url, group_id, encoded_msg);
    }

    LOG_DEBUG("发送群消息: %s", url);

    bool result = send_http_request(url);
    curl_free(encoded_msg);

    return result;
}

//发送自定义API请求
bool http_send_request(HTTPSender* sender, const char* api_path, const char* params) {
    if (!sender || !sender->enabled || !api_path) {
        return false;
    }

    char url[512];
    if (strlen(sender->access_token) > 0) {
        snprintf(url, sizeof(url), "%s/%s?access_token=%s&%s",
                sender->http_url, api_path, sender->access_token, params ? params : "");
    } else {
        snprintf(url, sizeof(url), "%s/%s?%s",
                sender->http_url, api_path, params ? params : "");
    }

    LOG_DEBUG("发送自定义请求: %s\n", url);

    return send_http_request(url);
}