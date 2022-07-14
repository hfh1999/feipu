/*一些定义，方便进行enum和string的转换，大量使用X宏*/
#define HTTP_METHOD_MAP(XX)                                                    \
  XX(0, HEAD, "HEAD")                                                          \
  XX(1, GET, "GET")                                                            \
  XX(2, DELETE, "DELETE")                                                      \
  XX(3, PUT, "PUT")                                                            \
  XX(4, POST, "POST")                                                          \
  XX(5, PATCH, "PATCH")

/*定义枚举*/
enum HttpMethod {
#define XX(num, name, string) HTTP_##name = num,
  HTTP_METHOD_MAP(XX)
#undef XX
      HTTP_UNEXPECT_METHOD
};

/*str 转换为enum型*/
enum HttpMethod http_method_enum(const char *str);
/*enum转换为str型*/
const char *http_method_str(enum HttpMethod);
/*判断str是否在枚举中*/
bool is_http_method_str(const char *str);
