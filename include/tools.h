#ifndef TOOL_H
#define TOOL_H
#include <vector>
namespace tool_helper {
}
namespace feipu {
    // 堆的实现

    using std::vector;
    template <typename T,typename Cmp>
    class Heap{
        public:
        Heap<T,Cmp>(){}
        void push(){}
        void pop(){}
        void size(){}
        private:
            vector<T> heap_;
    };
}
#endif