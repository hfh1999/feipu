#include "FeiTypes.h"
#include "HttpDef.h"
#include "tools.h"
#include <functional>
#include <list>
#include <unordered_map>
namespace feipu {
class HttpMsg;
class Service;
typedef std::function<void(const HttpMsg *, HttpMsg *)> HandleFuc;
struct HttpMethodHandler {
  HttpMethod method;
  HandleFuc handler;
  HttpMethodHandler() = default;
  HttpMethodHandler(HttpMethod m, const HandleFuc &h) : method(m), handler(h) {}
};
typedef std::list<HttpMethodHandler> HttpMethodHandlers;
// share_ptr是为了免除复制开销
typedef std::unordered_map<string, std::shared_ptr<HttpMethodHandlers>>
    HttpApiEntries;
class Router : noncopyable {
public:
  Router();
  Router &route(const string &path, HttpMethod method_route, HandleFuc handle);
  Router &route(const string &path, Service *service);

  // 内部使用
  void deal(HttpMsg *req, HttpMsg *resp); // 返回http回复的中间形式
private:
  HttpApiEntries entries_;
  HandleFuc match(const string &, HttpMsg *req);
  /*需要一个数据结构来记住各种route及handle*/
};
} // namespace feipu