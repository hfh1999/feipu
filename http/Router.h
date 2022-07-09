#include "tools.h"
#include "FeiTypes.h"
#include <functional>
namespace feipu {
class HttpMsg;
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

  // 内部使用
  HttpMsg* deal(const HttpMsg*); // 返回http回复的中间形式
  private:
  HandleFuc match(const string&);
  /*需要一个数据结构来记住各种route及handle*/

};
} // namespace feipu