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

add_executable(test_parse tests/test_parse.cpp)
target_link_libraries(test_parse feipu_http feipu)

add_executable(test_http tests/test_http.cpp)
target_link_libraries(test_http feipu_http feipu)

add_executable(test_httpmsg tests/test_HttpMsg.cpp)
target_link_libraries(test_httpmsg feipu_http feipu)