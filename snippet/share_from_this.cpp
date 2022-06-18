#include <memory>
#include <iostream>
using namespace std;
class A:public enable_shared_from_this<A> // 必须是public
{
    public:
    A(){
    }
    shared_ptr<int> ret_value(){
        return shared_ptr<int>(new int(5));
    }
    void test(){
        auto b = shared_from_this();
    }
};
int main()
{
    A a;
    /*https://stackoverflow.com/questions/27697973/shared-from-this-causing-bad-weak-ptr*/
    a.test(); // 会报错
    return 0;
}