#ifndef BUFFER_H
#define BUFFER_H
#include <FeiTypes.h>
#include <vector>
/*用于TcpConnection的一个类似于队列的buffer*/
/*线程不安全*/
/*TODO: 希望能改进使得不暴露buffer*/
/*TODO: 希望能改进实现zero copy*/
namespace feipu {
class Buffer {
public:
  Buffer();
  size_t getReadableBytes() { return writeIndex_ - readIndex_; }
  size_t getPrependableBytes() { return readIndex_; }
  const char *peek() const { return &*(data_.begin() + readIndex_); }
  void retrieve(size_t len);           //调用前需确认可读的大小
  void retrieveUntil(const char *end); //同样需要确认
  void retrieveAll();
  void replaceCRCF(size_t index); // 将index处的\r\n转换为\0\0

  void append(const string &str); // 向buffer中添加数据,无需确认可写的大小
  void append(const char* data,size_t len);
  ssize_t readFd(int fd);

  //Debug 用
  size_t ret_readIndex(){return readIndex_;}
  size_t ret_writeIndex(){return writeIndex_;}
  size_t ret_buffersize(){return data_.size();}

private:
  size_t getWriteableBytes() { return data_.size() - writeIndex_; }
  void makeSpace(size_t len);
  typedef std::vector<char>::size_type BufSizeType; // is size_t
  static const BufSizeType DefaultSize = 1024;
  static const BufSizeType DefaultPreSize = 8;
  std::vector<char> data_;
  BufSizeType readIndex_;
  BufSizeType writeIndex_;
};
} // namespace feipu
#endif