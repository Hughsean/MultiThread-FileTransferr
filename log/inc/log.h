//
// Created by xSeung on 2023/4/20.
//

#ifndef SERVER_LOG_H
#define SERVER_LOG_H

// #include <utility>

#include "cstdint"
#include "list"
#include "memory"
#include "ostream"
#include "string"

namespace cncd {
#define LOG(c)           LogEvent(__FILE__, __LINE__, (c))
#define LOGWITHID(c, id) LogEvent(__FILE__, __LINE__, (c), (id))

        enum class LogLevel { DEBUG = 1, INFO };

        // 日志事件/实体
        struct LogEvent {
                // using ptr = std::shared_ptr<LogEvent>;
                char const* m_file;      // 文件名
                uint32_t    m_line;      // 行号
                uint32_t    m_threadid;  // 线程id
                std::string m_content;   // 内容
                // LogLevel        m_level;
                LogEvent(const char* file, uint32_t line, const std::string&& content, uint32_t tid = UINT32_MAX);
        };
        // 格式化
        struct LogFormater {
                static std::string format(LogLevel level, const LogEvent& event);
        };
        // 日志输出target
        class LogAppender {
            public:
                using ptr = std::shared_ptr<LogAppender>;
                explicit LogAppender(std::ostream& ostm);
                void log(LogLevel level, const LogEvent&& event);

            private:
                std::ostream& m_os;
        };

}  // namespace cncd

#endif  // SERVER_LOG_H
