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
const char* http_status_str(enum HttpStatus status)
{
  switch (status) {
#define XX(num, name, string)                                                  \
  case HTTP_STATUS_##name:                                                            \
    return #string;
    HTTP_STATUS_MAP(XX)
#undef XX
  default:
    return "<unexpect>";
  }
}
enum HttpStatus http_status_enum(const char* str)
{
#define XX(num, name, string)                                                  \
  if (strcmp(#string, str) == 0)                                                \
    return HTTP_STATUS_##name;
    HTTP_STATUS_MAP(XX)
#undef XX
  return HTTP_UNEXPECT_STATUS;

}

const char* http_content_type_suffix(enum HttpContentType type)
{
  switch (type) {
#define XX(name, string, suffix)                                                  \
  case name:                                                            \
    return #suffix;
    HTTP_CONTENT_TYPE_MAP(XX)
#undef XX
  default:
    return "<unexpect>";
  }

}
const char* http_content_type_str_by_suffix(const char* str) {
#define XX(name, string, suffix) \
    if (strcmp(str, #suffix) == 0) { \
        return #string; \
    }
    HTTP_CONTENT_TYPE_MAP(XX)
#undef XX
    return "";
}

enum HttpContentType http_content_type_enum_by_suffix(const char* str)
{
  #define XX(name,string,suffix)\
  if(strcmp(str,#suffix) == 0){\
  return name; \
  }
  HTTP_CONTENT_TYPE_MAP(XX)
  #undef XX
  return CONTENT_TYPE_UNEXPECT;
}