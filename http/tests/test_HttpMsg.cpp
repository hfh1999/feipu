#include <iostream>
#include "HttpMsg.h"
#include <boost/any.hpp>
using namespace std;
using namespace feipu;
struct Handle{
    Handle():req_(new HttpRequest),res_(new HttpResponse){}
    Handle(const Handle& rhs):req_(rhs.req_),res_(rhs.res_){
    }
    ~Handle(){
        delete req_;
        delete res_;
    }
    HttpRequest* req_;
    HttpResponse* res_;
};
int main()
{
    boost::any hello((Handle()));
    cout << "success." <<endl;
}