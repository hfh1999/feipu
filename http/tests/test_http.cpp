#include "EventLoop.h"
#include "HttpServer.h"
#include "HttpMsg.h"
#include "Router.h"
using namespace feipu;
using namespace std;
int main()
{
    EventLoop loop;
    HttpServer myweb(&loop,InetAddress(8989),"test_web");
    Router my_service;
    my_service.route("/index.html",HttpMethod::HTTP_GET,[](const HttpRequest* req,HttpResponse * resp){
        resp->setContent(string("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Hello World</title></head><body><p>Hello HTML5</p></body></html>"));
    });
    myweb.start();
    loop.loop();
}