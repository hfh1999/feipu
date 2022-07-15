#include "Router.h"
#include "HttpMsg.h"
namespace feipu {
Router::Router() {}
Router &Router::route(const string &path, Routing method_route, HandleFuc) {
  return *this;
}
Router &Router::route(const string &path, Service *service) { return *this; }
void Router::deal(HttpMsg * req,HttpMsg * resp)
{
    // match HttpMsg and send to hanlefunc

    // 这里进行路径的处理
    auto func = match(req->url,req); // 还应把标出来的符号存入req

    func(req,resp);
}
Router::HandleFuc Router::match(const string&,HttpMsg* req)
{
  return HandleFuc();
}
} // namespace feipu