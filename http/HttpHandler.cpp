#include "HttpHandler.h"
#include "Buffer.h"
#include "HttpMsg.h"
#include "Router.h"
#include "TcpConnection.h"
#include <cstring>
#include <strings.h>
namespace feipu {
HttpHandler::HttpHandler()
    : router_(nullptr), check_state_(CHECK_STATE_REQUESTLINE),
      line_state_(LINE_OPEN), line_index_(0), req_(new HttpMsg),
      respon_(new HttpMsg) {}
HttpHandler::ParseStatus
HttpHandler::parse_http(Buffer *buffer) // 利用状态机解析
{

  while (true) {
    if (check_state_ == CHECK_STATE_CONTENT) // line_state_是否有必要?
    {
      // body数据无需经过line解析
    } else {
      line_state_ = parse_line(buffer);
    }

    if (line_state_ == LINE_BAD) // 不符行的规范
    {
      return PARSE_ERROR; // 会断开无需管理buffer的前进
    }
    if (line_state_ == LINE_OPEN) {
      return PARSE_RECV; // buffer无需前进因为这一行需要更多数据,向conection取
    }

    const char *line_text = buffer->peek();
    size_t readable_n = buffer->getReadableBytes();
    // LINE_OK
    switch (check_state_) {
    case CHECK_STATE_REQUESTLINE: {
      ParseStatus tmp_parse_state = parse_request_line(line_text, req_);
      if (PARSE_RECV == tmp_parse_state) // 需要更多数据,向缓存取,状态不变
        continue;
      buffer->retrieve(line_index_); //后面都无需buffer的缓存了
      line_index_ = 0;
      if (tmp_parse_state == PARSE_ERROR || tmp_parse_state == PARSE_END) {
        return tmp_parse_state; // error,end
      }
      check_state_ = CHECK_STATE_HEADER;
      break;
    }
    case CHECK_STATE_HEADER: {
      ParseStatus tmp_parse_state = parse_headers(line_text, req_);
      if (PARSE_RECV == tmp_parse_state) // 需要更多数据,向缓存取,状态不变
        continue;
      buffer->retrieve(line_index_); // 后面这几种情况都无需buffer的前一行数据了
      line_index_ = 0;
      if (tmp_parse_state == PARSE_ERROR || tmp_parse_state == PARSE_END) {
        return tmp_parse_state; // error,end
      }
      check_state_ = CHECK_STATE_CONTENT; // NEXT状态,切换状态
      break;
    }
    case CHECK_STATE_CONTENT: {
      ParseStatus tmp_parse_state = parse_content(line_text, readable_n, req_);
      if (tmp_parse_state == PARSE_RECV)
        return PARSE_RECV; // 需要获取更多的数据
      if (tmp_parse_state == PARSE_ERROR || tmp_parse_state == PARSE_END) {
        return tmp_parse_state; // error,end
      }
      assert(1); // 不可能到这里
      break;
    }
    default:
      return PARSE_ERROR; // 解析错误
      break;
    }
  }

  return PARSE_RECV;
  //需要更多数据
}
HttpHandler::LineStatus HttpHandler::parse_line(Buffer *buffer) {
  char tmp;
  size_t bufsize = buffer->getReadableBytes();
  for (; line_index_ < bufsize; line_index_++) {
    tmp = *(buffer->peek() + line_index_);
    if (tmp == '\r') {
      if (line_index_ == bufsize) {
        return LINE_OPEN;
      } else if (*(buffer->peek() + line_index_ + 1) == '\n') {
        buffer->replaceCRCF(line_index_);
        line_index_ += 2;
        return LINE_OK;
      } else {
        return LINE_BAD;
      }
    } else if (tmp == '\n') {
      if (line_index_ > 0 && *(buffer->peek() + line_index_ - 1) == '\r') {
        buffer->replaceCRCF(line_index_ - 1);
        line_index_ += 1;
        return LINE_OK;
      } else {
        return LINE_BAD;
      }
    }
  }

  return LINE_OPEN;
}

HttpHandler::ParseStatus HttpHandler::parse_request_line(const char *text,
                                                         HttpMsg *msg) {
  int start_index = 0;

  int index = start_index;
  while (*(text + index) != '\0' && *(text + index) != ' ')
    index++;
  if (*(text + index) == '\0')
    return PARSE_ERROR; // 提前结束
  string method(text + start_index, index - start_index);
  start_index = index + 1;
  if (!req_->setMethod(method)) {
    return PARSE_ERROR; // 不支持的方法
  }

  index = start_index;
  while (*(text + index) != '\0' && *(text + index) != ' ')
    index++;
  if (*(text + index) == '\0')
    return PARSE_ERROR; // 提前结束
  string url(text + start_index, index - start_index);
  start_index = index + 1;
  if (!req_->setUrl(url)) {
    return PARSE_ERROR; // 空url
  }

  index = start_index;
  while (*(text + index) != '\0')
    index++;
  string version(text + start_index, index - start_index);
  start_index = index + 1;
  if (!req_->setVersion(version)) {
    return PARSE_ERROR; // 不支持的版本
  }
  return PARSE_NEXT;
}
HttpHandler::ParseStatus HttpHandler::parse_headers(const char *text,
                                                    HttpMsg *msg) {
  if (text[0] == '\0') {
    if (req_->content_lenth != 0) {
      return PARSE_NEXT;
    }
    return PARSE_END;
  }
  if (strncasecmp(text, "Content-length:", 15) == 0) {
    text += 15;
    text += strspn(text, " \t"); // 跳过空字符
    (req_->headers)["Content-length"] = text;
    req_->content_lenth = atol(text);
    return PARSE_RECV;
  }

  // 其他字段只是简单的将其纳入到headers中
  const char *first_empty = strpbrk(text, " \t");
  if (first_empty == nullptr)
    return PARSE_ERROR; // key为空显然错误
  string key(text, first_empty);
  text = first_empty;
  text += strspn(text, " \t");
  string value(text);
  (req_->headers)[key] = value;
  return PARSE_RECV;
}
HttpHandler::ParseStatus HttpHandler::parse_content(const char *data,
                                                    size_t len, HttpMsg *msg) {
  // FIXME 应当设置超时,当较长时间没有收到数据会断开连接
  if (len >= req_->content_lenth) {
    req_->content_data.append(data, req_->content_lenth); // 复制了
    return PARSE_END;
  } else {
    return PARSE_RECV;
  }
}
void HttpHandler::handle_http(TcpConnectionPtr conn) {
  if (router_) {
    router_->deal(req_,respon_); // FIXME 使用智能指针
                                            // FIXME 过大时将不好使
    string data = respon_->dump();
    conn->send(data);
  }
}
} // namespace feipu