#include <iostream>
#include <list>
#include <memory>
using namespace std;
int main() {
  auto list_ptr = make_shared<list<int>>();
  auto it = list_ptr->begin();
  return 0;
}