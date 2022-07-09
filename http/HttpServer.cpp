#include "HttpServer.h"
#include "HttpMsg.h"
#include "Router.h"
#include "TcpServer.h"
#include "Buffer.h"
namespace feipu {
HttpServer::HttpServer(EventLoop *loop, InetAddress listenAddr, string name)
    : tcpserver_(new TcpServer(loop, listenAddr, name)), router_(nullptr),
      check_state_(CHECK_STATE_REQUESTLINE), line_state_(LINE_OPEN),
      line_index_(0),
      building_msg_(new HttpMsg) {
  tcpserver_->setMessageCallback(
      std::bind(&HttpServer::parse_http, this, _1, _2));
}
HttpServer::~HttpServer() {}
void HttpServer::parse_http(TcpConnectionPtr conn, Buffer *buffer) {
  line_state_ = parse_line(buffer);
  const char* line_text = buffer->peek();
  if (line_state_ == LINE_OK) {
    switch (check_state_) {
    case CHECK_STATE_REQUESTLINE: {
      parse_request_line(line_text, building_msg_);
      break;
    }
    case CHECK_STATE_HEADER: {
      parse_headers(line_text, building_msg_);
      break;
    }
    case CHECK_STATE_CONTENT: {
      parse_content(line_text, building_msg_);
      break;
    }
    default:
      // 解析错误
      break;
    }
  }
  // do nothing
  //需要更多数据
}
HttpServer::LineStatus HttpServer::parse_line(Buffer * buffer) {
    char tmp;
    size_t bufsize = buffer->getReadableBytes();
    for(; line_index_ < bufsize;line_index_++)
    {
        tmp = *(buffer->peek() + line_index_);
        if(tmp == '\r')
        {
            if(line_index_ == bufsize)
            {
                return LINE_OPEN;
            }
            else if(*(buffer->peek() + line_index_ + 1) == '\n')
            {
                buffer->replaceCRCF(line_index_);
                line_index_ += 2;
                return LINE_OK;
            }
            else
            {
                return LINE_BAD;
            }
        }
        else if(tmp == '\n')
        {
            if(line_index_ > 0 && *(buffer->peek() + line_index_) == '\r')
            {
                buffer->replaceCRCF(line_index_ - 1);
                line_index_ += 1;
                return LINE_OK;
            }
            else
            {
                return LINE_BAD;
            }
        }
    }

    return LINE_OPEN;
}
bool HttpServer::is_one_of_method(const string& in_method_str)
{
  if (in_method_str == "HEAD" || in_method_str == "GET" ||
      in_method_str == "DELETE" || in_method_str == "PUT" ||
      in_method_str == "POST" || in_method_str == "PATCH") {
    return true;
  } else {
    return false;
  }
}
void HttpServer::parse_request_line(const char* text, HttpMsg *msg) {
    int start_index = 0;

    int index = start_index;
    while(*(text+index) != '\0' && *(text+index) != ' ') index++;
    if(*(text+index) == '\0')
        return; // error
    string method(text + start_index, index - start_index);
    start_index = index + 1;
    if (!is_one_of_method(method)) { return; }

    index = start_index;
    while(*(text+index) != '\0' && *(text+index) != ' ') index++;
    if(*(text+index) == '\0')
        return; // error
    string url(text + start_index,index - start_index);
    start_index = index + 1;
    if(url.empty()) {return;}

    index = start_index;
    while(*(text+index) != '\0') index++;
    string version(text + start_index,index - start_index);
    start_index = index + 1;
    if(url != "HTTP/1.1") {return;}
    
}
void HttpServer::parse_headers(const char* text, HttpMsg *msg) {}
void HttpServer::parse_content(const char*, HttpMsg *msg) {}
void HttpServer::response_http(TcpConnectionPtr conn, const HttpMsg *msg) {
  if (router_) {
    HttpMsg *response = router_->deal(msg); // FIXME 使用智能指针
                                            // FIXME 过大时将不好使
    string data = response->dump();
    conn->send(data);
  }
}
void HttpServer::serve(Router *in_app) { router_ = in_app; }
} // namespace feipu