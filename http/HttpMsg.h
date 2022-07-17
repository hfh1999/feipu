#ifndef HTTPMSG_H
#define HTTPMSG_H
#include "FeiTypes.h"
#include "HttpDef.h"
#include <map>
namespace feipu {
// 解析后的http信息
class HttpMsg {
public:
  typedef std::map<string, string> HttpHeaders;
  typedef std::map<string, string> PathMatches;

  string path;
  PathMatches path_matches; // 匹配的path中的部分
  HttpMethod method;
  HttpHeaders headers; // string-string map
  string version;

  size_t content_lenth;
  string content_data;
  bool setMethod(const string &method_str);
  bool setUrl(const string &url_str);
  bool setVersion(const string &version_str);
  // header
  HttpMsg();
  string dump();
};
} // namespace feipu
#endif