#include "HttpMsg.h"
namespace feipu {
    HttpMsg::HttpMsg()
    :content_lenth(0){}
    bool HttpMsg::setMethod(const string &method_str)
    {
        HttpMethod tmp_method = http_method_enum(method_str.c_str());
        if(tmp_method == HttpMethod::HTTP_UNEXPECT_METHOD)
        {
            return false;
        }
        else
        {
            method = tmp_method;
            return true;
        }
    }

    bool HttpMsg::setUrl(const string &url_str) {
      if (url_str.empty()) {
        return false;
      } else {
        url = url_str;
        return true;
      }
    }
  bool HttpMsg::setVersion(const string& version_str)
  {
    if(version == "HTTP/1.1") // 只支持1.1
    {
        version = version_str;
        return true;
    }
    else
    {
        return false;
    }
  }
}