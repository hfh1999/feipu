#ifndef HTTPMSG_H
#define HTTPMSG_H
#include "FeiTypes.h"
#include "HttpDef.h"
#include <map>
namespace feipu {
// 解析后的http信息
struct HttpMsg {
  typedef std::map<string, string> HttpHeaders;
  typedef std::map<string, string> PathMatches;

  HttpMsg();
  virtual ~HttpMsg();
  //string path;
  //PathMatches path_matches; // 匹配的path中的部分
  //HttpMethod method;
  HttpHeaders headers; // string-string map
  string version;

  string content_data;
  size_t content_lenth;
  bool setVersion(const string &version_str);
  // header
  void DumpHeaders(string& in_str);
  void DumpBody(string& in_str);
  virtual string Dump(bool is_dump_headers, bool is_dump_body) = 0;
};

class HttpRequest: public HttpMsg{
  public:
  string path;
  PathMatches path_matches; // 匹配的path中的部分
  HttpMethod method;
  bool setMethod(const string &method_str);
  bool setUrl(const string &url_str);
  string Dump(bool is_dump_headers,bool is_dump_body) override;// 将消息序列化
};
class HttpResponse: public HttpMsg{
  public:
  HttpStatus status_code;
  string Dump(bool is_dump_headers,bool is_dump_body) override;// 将消息序列化
};
} // namespace feipu
#endif