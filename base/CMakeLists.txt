set(feipu_SRCS
src/Eventloop.cpp
src/Channel.cpp 
src/Logging.cpp 
src/CurrentThread.cpp 
src/Thread.cpp
src/FeiSocketops.cpp
src/InetAddress.cpp
src/TcpServer.cpp
src/TcpConnection.cpp
src/Buffer.cpp
src/EventLoopPool.cpp
src/EventLoopThread.cpp
src/UdpServer.cpp
src/Poller.cpp
)
add_library(feipu ${feipu_SRCS})