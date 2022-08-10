#include "Router.h"
#include "HttpMsg.h"
#include "Logging.h"
namespace feipu {
Router::Router() {}
Router &Router::route(const string &path, HttpMethod method_route,
                      HandleFuc handler) {
  // todo 需要检查通配符
  std::shared_ptr<HttpMethodHandlers> handlers_ptr;
  if (entries_.find(path) != entries_.end()) {
    handlers_ptr = std::make_shared<HttpMethodHandlers>();
  } else {
    handlers_ptr = entries_[path];
  }

  for (auto it = handlers_ptr->begin(); it != handlers_ptr->end(); it++) {
    if (it->method == method_route) {
      LOG_FATAL << "Router: path and method cannot be dumplicate at same time.";
    }
  }
  handlers_ptr->push_back(HttpMethodHandler(method_route, handler));
  return *this;
}
Router &Router::route(const string &path, Service *service) { return *this; }
void Router::deal(HttpRequest *req, HttpResponse *resp) {
  // match HttpMsg and send to hanlefunc

  // 这里进行路径的处理
  auto func = match(req->path, req); // 还应把标出来的符号存入req

  if(func)
  {
    LOG_TRACE << "req->path: => exec func ";
    func(req, resp);
  }
  else
  {
    // 找不到，未实现
    resp->status_code = HTTP_STATUS_NOT_IMPLEMENTED;
  }
}
HandleFuc Router::match(const string &, HttpRequest *req) {
  /*进行path和method匹配，匹配上了则执行相应的HandleFuc*/
  const char *s = req->path.c_str();
  const char *e = req->path.c_str();
  while (*e && *e != '?') // 只看到'?'之前的字符串.
    e++;

  string path = string(s, e);
  const char *kp,*ks,*vp,*vs;
  for(auto it = entries_.begin(); it != entries_.end(); it++)
  {
    kp = it->first.c_str();
    vp = path.c_str();

    std::map<string,string> matches;

    while(*kp && *vp) // 对每个entries的path进行匹配
    {
      if(*kp != *vp)
      {
        break;
      }
      else if(kp[0] == '/' && kp[1] == '*')
      {
        // 匹配到末尾
        kp += 2;
        ks = kp;
        vp += 1;
        vs = vp;
        while (*kp && *kp != '/') {kp++;}
        if(*kp == '/' && kp[1] != '\0') {
          LOG_FATAL << "Route: `*` usage error.";
        }
        while(*vp) {vp++;}
        matches[string(ks,kp - ks)] = string(vs,vp-vs);
      }
      else if(kp[0] == '/' && (kp[1] == ':' || kp[1] == '{'))
      {
        kp += 1;
        ks = kp;
        while(*kp && *kp != '/'){++kp;}
        vp += 1;
        vs = vp;
        while(*vp && *vp != '/'){++vp;}
        int klen = kp - ks;
        if(*(ks - 1) == '{' && *(kp - 1) == '}')
        {
          klen--;// {feild}
        }
        matches[string(ks,klen)] = string(vs,vp-vs);
      }
      else {
        kp++;
        vp++;
      }
    }
    bool is_match = false;
    if(!(*vp)&&!(*kp))
    {
      is_match = true;
    }

    if (is_match) {
      auto method_handlers = it->second;
      for(auto iter = method_handlers->begin(); iter != method_handlers->end();iter++)
      {
        if(iter->method == req->method)
        {
          for(auto& match:matches)
          {
            req->path_matches[match.first] = match.second;
          }
          return iter->handler; // 匹配到了
        }
      }
    } else {
      LOG_TRACE << "Route : No match.";
    }
  }
  return HandleFuc(); // 没有匹配到
}
} // namespace feipu