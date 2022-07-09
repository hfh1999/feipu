#include "FeiTypes.h"
namespace feipu {
    // 解析后的http信息
    class HttpMsg{
        public:
        string url;
        // header
        HttpMsg();
        string dump();
    };
}