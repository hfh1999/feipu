#include "EventLoop.h"
#include "HttpMsg.h"
#include "HttpServer.h"
#include "Logging.h"
#include "Router.h"
using namespace feipu;
using namespace std;
int main() {
  // Logger::setLogLevel(Logger::LoggerLevel::TRACE);
  EventLoop loop;
  HttpServer myweb(&loop, InetAddress(8989), "test_web");
  myweb.setThreadNum(6);
  Router my_service;
  my_service.route(
      "/index.html", HttpMethod::HTTP_GET,
      [](const HttpRequest *req, HttpResponse *resp) {
        resp->setContent(string(
            "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Hello "
            "World</title></head><body><p>Hello HTML5</p></body></html>"));
      });
  myweb.serve(&my_service);
  myweb.start();
  loop.loop();
}