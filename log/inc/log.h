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
        enum class LogLevel { DEBUG = 1, INFO };
        // 日志事件/实体
        struct LogEvent {
                using ptr                  = std::shared_ptr<LogEvent>;
                char const*     m_file     = nullptr;  // 文件名
                uint32_t        m_line     = 0;        // 行号
                uint32_t        m_threadid = 0;        // 线程id
                std::string     m_content;             // 内容
                LogLevel        m_level;
                static LogEvent make(const char* file, uint32_t line,
                                     const std::string&& content,
                                     uint32_t            tid = UINT32_MAX);
        };
        // 格式化
        class LogFormater {
            public:
                static std::string format(const LogEvent& event);
        };
        // 日志输出target
        class LogAppender {
            public:
                using ptr = std::shared_ptr<LogAppender>;
                explicit LogAppender(/*LogLevel level,*/ std::ostream& ostm);
                void log(LogLevel level, const LogEvent&& event);

            private:
                std::ostream& m_os;
        };

}  // namespace cncd

#endif  // SERVER_LOG_H
