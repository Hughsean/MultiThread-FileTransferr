//
// Created by xSeung on 2023/4/20.
//
#include "log.h"
#include "fmt/chrono.h"
#include "fmt/core.h"
#include "fstream"

namespace cncd {
        LogEvent::LogEvent(const char* file, uint32_t line, const std::string&& content, uint32_t tid) {
                m_file     = file;
                m_line     = line;
                m_threadid = tid;
                m_content  = content;
        }

        LogAppender::LogAppender(std::ostream& ostm) : m_os(ostm) {}

        void LogAppender::log(cncd::LogLevel level, const LogEvent&& event) {
                m_os << LogFormater::format(level, event);
                m_os.flush();
        }

        std::string LogFormater::format(LogLevel level, const LogEvent& event) {
                std::string str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr)));
                if (level == LogLevel::DEBUG) {
                        str = fmt::format("{} line:{} file:{}", str, event.m_line, event.m_file);
                }
                if (event.m_threadid != UINT32_MAX) {
                        str = fmt::format("[{} tid:{}]", str, event.m_threadid);
                }
                else {
                        str = fmt::format("[{}]", str);
                }
                return fmt::format("{}: {}", str, event.m_content);
        }
}  // namespace cncd