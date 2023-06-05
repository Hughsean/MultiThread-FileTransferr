
#ifndef CN_CD_CONFIG_H
#define CN_CD_CONFIG_H
#include "string"

namespace mtft {
/**
 * @brief 一些常量配置
 *
 * @version 0.1
 * @author 鳯玖 (xSeung@163.com)
 * @date 2023-04-23
 * @copyright Copyright (c) 2023
 */
#define FilePartName "{}_{}.part"
    extern const int                          BUFFSIZE;
    extern const int                          THREAD_N;
    extern const int                          JSONSIZE;
    extern const int                          UDPPORT;
    extern const int                          TCPPORT;
    extern const int                          CORESPONSEPORT;
    extern const int                          TIMEOUT;
    extern const int                          RECONNECTTIME;
    extern const int                          ParallelN;
    extern const std::string                  DIR;
    extern const std::string                  PROGRESS;
    extern const std::string                  TYPE;
    extern const std::string                  RESPONSE;
    extern const std::string                  SCAN;
    extern const std::string                  FILENAME;
    extern const std::string                  FILESIZE;
    extern const std::string                  ID;
    extern const std::string                  PORT;

    /**
     * @brief 禁止移动, 禁止复制
     *
     * @version 0.1
     * @author 鳯玖 (xSeung@163.com)
     * @date 2023-05-11
     * @copyright Copyright (c) 2023
     */
    class Base {
    public:
        Base(Base &)             = delete;
        Base(Base &&)            = delete;
        Base &operator=(Base &)  = delete;
        Base &operator=(Base &&) = delete;
        Base()                   = default;
    };
}  // namespace mtft
#endif  // CN_CD_CONFIG_H
