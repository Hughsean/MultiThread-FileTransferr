//
// Created by xSeung on 2023/4/21.
//

#ifndef CN_CD_FILEBLOCK_H
#define CN_CD_FILEBLOCK_H

#include "asio.hpp"
// #include "cstdint"
#include "config.h"
// #include "minwindef.h"
#include "fstream"
#include "log.h"
#include "mutex"
#include "string"

namespace cncd {
        using namespace asio;
        class FileBlock {
            public:
                using ptr = std::shared_ptr<FileBlock>;
                FileBlock(int id, Type type, const std::string&& str, uint32_t offset,
                          uint32_t length, LogAppender::ptr log);
                bool     isFinished() const;                    // 是否写/读完
                uint32_t read(BYTE* data, size_t buffersize);   // 读
                uint32_t write(BYTE* data, size_t buffersize);  // 写
                int      getID() const;                         //
            private:                                            //
                int                  m_id;                      // 块号
                LogAppender::ptr     m_log;                     //
                std::string          m_file;                    // 访问的文件
                static std::mutex    m_mutex;                   // 写互斥锁
                static std::ofstream m_ofs;                     // 输出流
                std::ifstream        m_ifs;                     // 输入流
                Type                 m_type;                    // 块类型
                uint32_t             m_offset;                  // 块起始偏移
                uint32_t             m_length;                  // 块内大小
                uint32_t             m_progress;                // 块内偏移
        };
        class FileReader {
            public:
                /// @brief          文件块读取构造函数
                /// @param id       块id
                /// @param fname    文件名称
                /// @param offset   块偏移
                /// @param length   块长度
                /// @param log      日志器
                FileReader(int id, const std::string&& fname, uint32_t offset,
                           uint32_t length, LogAppender::ptr log);
                /// @brief 返回是否读取完毕
                /// @return 读取完成:true;否则:false
                bool     finished();
                /// @brief 读取字节流
                /// @param data 
                /// @param buffersize 
                /// @return 
                uint32_t read(BYTE* data, uint32_t buffersize);
                int      getID();

            private:
                int              m_id;
                std::string      m_file_name;
                std::ifstream    m_ifs;
                uint32_t         m_offect;
                uint32_t         m_length;
                uint32_t         m_progress;
                LogAppender::ptr m_log;
        };
}  // namespace cncd
#endif  // CN_CD_FILEBLOCK_H
