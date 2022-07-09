#include "Buffer.h"
#include <Logging.h>
#include <cassert>
#include <sys/uio.h>
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
void Buffer::replaceCRCF(size_t index)
{
  assert(index+1 < writeIndex_);
  assert(index >= readIndex_);
  assert(data_[index] == '\r');
  assert(data_[index+1] == '\n');
  data_[index] = '\0';
  data_[index+1] = '\0';
}
void Buffer::append(const string &str) { append(str.data(), str.size()); }
void Buffer::append(const char *data, size_t len) {
  if (len > getWriteableBytes()) {
    // 空间不足需要调整
    makeSpace(len);
  }
  std::copy(data, data + len, data_.begin() + writeIndex_);
  writeIndex_ += len;
}
void Buffer::makeSpace(size_t len) {
  //若当前的空间足够，通过前移数据来实现实现makespace
  if (getWriteableBytes() + readIndex_ - DefaultPreSize >= len) {
    std::copy(data_.begin() + readIndex_, data_.begin() + writeIndex_,
              data_.begin() + DefaultPreSize);
    LOG_TRACE << "makespace move ---: "  << writeIndex_ << "," <<readIndex_;
    writeIndex_ = getReadableBytes() + DefaultPreSize;
    readIndex_ = DefaultPreSize;
    LOG_TRACE << "makespace move +++: "  << writeIndex_ << "," <<readIndex_;
  } else // 不够的话需要resize;
  {
    data_.resize(writeIndex_ + len);
  }
}
ssize_t Buffer::readFd(int fd) {
  char extrabuf[65536];
  struct iovec vec[2];
  vec[0].iov_base = &*(data_.begin() + writeIndex_);
  vec[0].iov_len = getWriteableBytes();
  vec[1].iov_base = &extrabuf;
  vec[1].iov_len = sizeof(extrabuf);
  int n = ::readv(fd, vec, 2);
  if (n < 0) {
    LOG_SYSERROR << "Buffer: readv error.";
  } else if (n <= static_cast<int>(getWriteableBytes())) {
    writeIndex_ += n;
    LOG_TRACE <<"n = " << n;
  } else {
    size_t oldWriteable = getWriteableBytes();
    writeIndex_ = data_.size();
    append(extrabuf, n - oldWriteable);
  }
  return n;
}
} // namespace feipu