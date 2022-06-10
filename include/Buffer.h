#ifndef BUFFER_H
#define BUFFER_H
/*用于TcpConnection的一个类似于队列的buffer*/
/*TODO: 希望能改进使得不暴露buffer*/
#include <vector>
namespace feipu {
    class Buffer{
        public:
            Buffer(){}
        private:
        std::vector<char> data_;
    };
}
#endif