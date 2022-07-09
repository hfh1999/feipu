#include "CallBack.h"
#include "FeiTypes.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "tools.h"
namespace feipu {
class TcpServer;
class EventLoop;
class Router;
class HttpMsg;
class HttpServer : noncopyable {
public:
  HttpServer(EventLoop *loop, InetAddress listenAddr, string name);
  ~HttpServer();

private:
enum Method{
    HEAD,
    GET,
    DELETE,
    PUT,
    POST,
    PATCH
};
bool is_one_of_method(const string& in_method_str);
  // 请求报文的解析状态
  enum RequestState {
    CHECK_STATE_REQUESTLINE = 0, // 请求行解析中
    CHECK_STATE_HEADER,          // 头部解析中
    CHECK_STATE_CONTENT          // 内容(content)解析中
  };

  // 行的解析状态
  enum LineStatus { LINE_OK = 0, LINE_BAD, LINE_OPEN };
  /* 绑定到tcpserver的read
      解析完整的信息后，产生内部表示，然后调用Route
  */
  void parse_http(TcpConnectionPtr, Buffer *); // 利用状态机解析
  LineStatus parse_line(Buffer*);

  /* 各种parse*/
  void parse_request_line(const char* text,HttpMsg* msg);
  void parse_headers(const char* text,HttpMsg* msg);
  void parse_content(const char* text,HttpMsg* msg);

  void response_http(
      TcpConnectionPtr conn,
      const HttpMsg *msg); // 解析完将中间形式发给router进行处理,然后发送
  void deal_and_send_over();  // 发送完毕
  void serve(Router *in_app); // 注册router

  std::unique_ptr<TcpServer> tcpserver_;
  Router *router_;
  RequestState check_state_;
  LineStatus line_state_;
  size_t line_index_;// 记住当前分析的行分析到哪了
  HttpMsg* building_msg_; // http信息的中间表示
};
} // namespace feipu