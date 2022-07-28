#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H
#include "CallBack.h"
namespace feipu {
class Buffer;
class HttpMsg;
class HttpRequest;
class HttpResponse;
class Router;
/*注意这个类不是线程安全的*/
class HttpHandler {
public:
  HttpHandler();
  void init(Router *router) { router_ = router; };
  enum ParseStatus {
    PARSE_NEXT,  // 阶段解析解析完成，到下一个阶段
    PARSE_ERROR, // 解析错误
    PARSE_RECV,  // 需要更多数据
    PARSE_END    // 提前跳出
  };
  ParseStatus parse_http(Buffer *buffer);
  void handle_http(
      TcpConnectionPtr conn); // 解析完将中间形式发给router进行处理,然后发送

private:
  enum Method { HEAD, GET, DELETE, PUT, POST, PATCH };
  // 请求报文的解析状态
  enum RequestState {
    CHECK_STATE_REQUESTLINE = 0, // 请求行解析中
    CHECK_STATE_HEADER,          // 头部解析中
    CHECK_STATE_CONTENT          // 内容(content)解析中
  };

  // 行的解析状态
  enum LineStatus { LINE_OK = 0, LINE_BAD, LINE_OPEN };
  LineStatus parse_line(Buffer *);
  size_t line_end_index_; // 只是方便获取一行末尾的位置而已

  /* 各种parse*/
  ParseStatus parse_request_line(const char *text, HttpRequest *msg); // 读入字符串
  ParseStatus parse_headers(const char *text, HttpRequest *msg); // 读入字符串
  ParseStatus parse_content(const char *data, size_t len, HttpRequest *msg);
  Router *router_;
  RequestState check_state_;
  LineStatus line_state_;
  size_t line_index_; // 记住当前分析的行分析到哪了
  HttpRequest *req_;      // 解析后的http信息
  HttpResponse *respon_;   // 构造后的http信息
};
} // namespace feipu
#endif