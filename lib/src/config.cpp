#include "config.h"
namespace mtft {
    const int                          THREAD_N       = 3;
    const int                          BUFFSIZE       = 1024;
    const int                          JSONSIZE       = 5120;
    const int                          UDPPORT        = 8080;
    const int                          TCPPORT        = 9090;
    const int                          CORESPONSEPORT = 9999;
    const int                          TIMEOUT        = 2000;
    const int                          RECONNECTTIME  = 3000;
    const int                          ParallelN      = 3;
    const std::string                  DIR            = ".tmp";
    const std::string                  PROGRESS       = "progress";
    const std::string                  TYPE           = "TYPE";
    const std::string                  SCAN           = "SCAN";
    const std::string                  RESPONSE       = "RESPONSE";
    const std::string                  FILENAME       = "FIELNAME";
    const std::string                  FILESIZE       = "FILESIZE";
    const std::string                  ID             = "ID";
    const std::string                  PORT           = "PORT";
    [[maybe_unused]] const std::string SEND           = "SEND";
    [[maybe_unused]] const std::string OK             = "OK";
    [[maybe_unused]] const std::string HOSTNAME       = "HOSTNAME";
}  // namespace mtft