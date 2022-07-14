#include "HttpDef.h"
#include <cstring>
/*str 转换为enum型*/
enum HttpMethod http_method_enum(const char *str) {
#define XX(num, name, string)                                                  \
  if (strcmp(string, str) == 0)                                                \
    return HTTP_##name;
  HTTP_METHOD_MAP(XX)
#undef XX
  return HTTP_UNEXPECT_METHOD;
}
/*enum转换为str型*/
const char *http_method_str(HttpMethod method) {
  switch (method) {
#define XX(num, name, string)                                                  \
  case HTTP_##name:                                                            \
    return string;
    HTTP_METHOD_MAP(XX)
#undef XX
  default:
    return "<unexpect>";
  }
}
/*判断str是否在枚举中*/
bool is_http_method_str(const char *str) {
#define XX(num, name, string)                                                  \
  if (strcmp(string, str) == 0)                                                \
    return true;
  HTTP_METHOD_MAP(XX)
#undef XX
  return false;
}