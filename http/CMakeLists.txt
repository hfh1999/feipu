set (feipu_http_SRCS
    HttpDef.cpp
    HttpHandler.cpp
    HttpMsg.cpp
    HttpServer.cpp
    Router.cpp
    Service.cpp
)

add_library(feipu_http ${feipu_http_SRCS})
target_link_libraries(feipu_http feipu)