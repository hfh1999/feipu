#include "Buffer.h"
#include <cassert>
namespace feipu {
Buffer::Buffer()
    : data_(DefaultSize + DefaultPreSize), readIndex_(DefaultPreSize),
      writeIndex_(readIndex_) {}
void Buffer::retrieve(size_t len) {
  assert(len <= getReadableBytes());
  readIndex_ += len;
}
void Buffer::retrieveUntil(const char *end) {
  const char *startOfWrite = &*(data_.begin() + writeIndex_);
  assert(end <= startOfWrite);
  assert(end >= peek());
  retrieve(end - peek());
}
void Buffer::append(const string &str) {
    append(str.data(),str.size());
}
void Buffer::append(const char* data,size_t len){
  if (len > getWriteableBytes()) {
    // 空间不足需要调整
    makeSpace(len);
  }
  std::copy(data, data + len, data_.begin() + writeIndex_);
}
void Buffer::makeSpace(size_t len) {
    //若当前的空间足够，通过前移数据来实现实现makespace
    if(getWriteableBytes() + readIndex_ - DefaultPreSize >= len)
    {
      std::copy(data_.begin() + readIndex_, data_.begin() + writeIndex_,
                data_.begin() + DefaultPreSize);
      writeIndex_ = getReadableBytes() + DefaultPreSize; 
      readIndex_ = DefaultPreSize;
    }
    else // 不够的话需要resize;
    {
        data_.resize(writeIndex_ + len);
    }
}
} // namespace feipu