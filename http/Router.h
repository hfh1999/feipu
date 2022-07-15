#include "tools.h"
#include "FeiTypes.h"
#include <functional>
namespace feipu {
class HttpMsg;
class Service;
class Router : noncopyable {
public:
enum Routing{
    HEAD,
    GET,
    DELETE,
    PUT,
    POST,
    PATCH
};
  typedef std::function<void(const HttpMsg*,HttpMsg*)> HandleFuc;
  Router();
  Router& route(const string& path,Routing method_route,HandleFuc handle);
  Router& route(const string& path,Service* service);

  // 内部使用
  void deal(HttpMsg* req, HttpMsg* resp); // 返回http回复的中间形式
  private:
  HandleFuc match(const string&,HttpMsg* req);
  /*需要一个数据结构来记住各种route及handle*/

};
} // namespace feipu