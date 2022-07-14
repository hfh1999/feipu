#include "Router.h"
#include "HttpMsg.h"
namespace feipu {
Router::Router() {}
Router &Router::route(const string &path, Routing method_route, HandleFuc) {
  return *this;
}
Router &Router::route(const string &path, Service *service) { return *this; }
HttpMsg* Router::deal(const HttpMsg * in_msg)
{
    // match HttpMsg and send to hanlefunc
    auto func = match(in_msg->url);
    HttpMsg* out_msg = new(HttpMsg); // FIXME 使用智能指针
    func(in_msg,out_msg);
    return out_msg;
}
} // namespace feipu