#include "FeiTypes.h"
#include "HttpDef.h"
#include <map>
namespace feipu {
// 解析后的http信息
class HttpMsg {
public:
  typedef std::map<string,string> http_headers;

  string url;
  HttpMethod method;
  http_headers headers; // string-string map
  string version;

  size_t content_lenth;
  string content_data;
  bool setMethod(const string& method_str);
  bool setUrl(const string& url_str);
  bool setVersion(const string& version_str);
  // header
  HttpMsg();
  string dump();
    };
}