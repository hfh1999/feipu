#include "HttpMsg.h"
namespace feipu {
bool HttpMsg::setVersion(const string &version_str) {
  if (version_str == "HTTP/1.1") // 只支持1.1
  {
    version = version_str;
    return true;
  } else {
    return false;
  }
}

HttpMsg::HttpMsg() : content_lenth(0) {}
HttpMsg::~HttpMsg() {}
bool HttpRequest::setMethod(const string &method_str) {
  HttpMethod tmp_method = http_method_enum(method_str.c_str());
  if (tmp_method == HttpMethod::HTTP_UNEXPECT_METHOD) {
    return false;
  } else {
    method = tmp_method;
    return true;
  }
}
void HttpMsg::setContent(const string& in_data)
{
  setContent(in_data.data(),in_data.size());
}
void HttpMsg::setContent(const char* in_data,size_t len)
{
  content_data.append(in_data,len);
  headers["Content-length"] = std::to_string(len);
}
void HttpMsg::DumpHeaders(string &in_str) {
  string tmp_str;
  for(auto header:headers)
  {
    tmp_str.append(header.first + ":" + " " + header.second + "\r\n");
  }
}
void HttpMsg::DumpBody(string &in_str) {
  in_str.append(content_data);
}
string HttpMsg::Dump(bool is_dump_headers, bool is_dump_body)
{
  // 默认行为,为httpmsg 序列化headers和body
  string str;
  if(is_dump_headers)
  {
    DumpHeaders(str);
  }
  str += "\r\n"; // 空行
  if(is_dump_body)
  {
    DumpBody(str);
  }
  return str;
}

bool HttpRequest::setUrl(const string &url_str) {
  if (url_str.empty()) {
    return false;
  } else {
    path = url_str;
    return true;
  }
}
string HttpRequest::Dump(bool is_dump_headers, bool is_dump_body)
{
  // TODO  需要将结构化的url序列化,url是否需要更复杂的表示?
  string str;
  str.append(string_format("%s %s HTTP/1.1\r\n", http_method_str(method),
                           path.c_str()));
  str += HttpMsg::Dump(is_dump_headers,is_dump_body);
  return str;
}
string HttpResponse::Dump(bool is_dump_headers, bool is_dump_body)
{
  string str;
  str.append(string_format("HTTP/1.1 %d %s\r\n",status_code,http_status_str(status_code)));
  str += HttpMsg::Dump(is_dump_headers,is_dump_body);
  return str;
}
} // namespace feipu