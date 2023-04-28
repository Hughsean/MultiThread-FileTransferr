//
// Created by xSeung on 2023/4/21.
//

#ifndef CN_CD_FILEBLOCK_H
#define CN_CD_FILEBLOCK_H

#include "asio.hpp"
#include "config.h"
#include "fstream"
#include "log.h"
#include "mutex"
#include "string"

namespace cncd {
        using namespace asio;
        // class FileBlock {
        //     public:
        //         using ptr = std::shared_ptr<FileBlock>;
        //         enum class Type { DOWNLOAD, UPLOAD };
        //         FileBlock(int id, Type type, const std::string&& str, uint32_t
        //         offset,
        //                   uint32_t length, LogAppender::ptr log);
        //         bool     isFinished() const;                    // 是否写/读完
        //         uint32_t read(BYTE* data, size_t buffersize);   // 读
        //         uint32_t write(BYTE* data, size_t buffersize);  // 写
        //         int      getID() const;                         //
        //     private:                                            //
        //         int                  m_id;                      // 块号
        //         LogAppender::ptr     m_log;                     //
        //         std::string          m_file;                    // 访问的文件
        //         static std::mutex    m_mutex;                   // 写互斥锁
        //         static std::ofstream m_ofs;                     // 输出流
        //         std::ifstream        m_ifs;                     // 输入流
        //         Type                 m_type;                    // 块类型
        //         uint32_t             m_offset;                  // 块起始偏移
        //         uint32_t             m_length;                  // 块内大小
        //         uint32_t             m_progress;                // 块内偏移
        // };

        /*--------------------------------------------------------------------------*/
        class FileReader {
            public:
                using ptr = std::shared_ptr<FileReader>;
                /// @brief          文件块读取构造函数
                /// @param id       块id
                /// @param fpath    文件地址
                /// @param offset   块偏移
                /// @param length   块长度
                /// @param log      日志器
                FileReader(int id, const std::string& fpath, uint32_t offset, uint32_t length, LogAppender::ptr log);
                /// @brief 返回是否读取完毕
                /// @return 读取完成:true;否则:false
                bool finished();
                /// @brief 读取字节流
                /// @param data
                /// @param buffersize
                /// @return
                uint32_t read(BYTE* data, uint32_t buffersize);
                /// @brief 取得当前块id
                /// @return
                int                     getID();
                static std::vector<ptr> ReadersBuilder(int n, const std::string& pathWithfname, LogAppender::ptr log);

            private:
                const int        mID;
                std::string      mPathWithFileName;
                std::ifstream    mifs;
                uint32_t         mOffect;
                uint32_t         mLength;
                uint32_t         mProgress;
                LogAppender::ptr mlog;
        };
        class FileWriter {
            public:
                using ptr = std::shared_ptr<FileWriter>;
                FileWriter(int id, const std::string& filename, const std::string& path, uint32_t offset,
                           uint32_t length, LogAppender::ptr log);
                bool                    finished();
                uint32_t                write(BYTE* data, uint32_t buffersize);
                int                     getID();
                const std::string&      getFname();
                static std::vector<ptr> fwsCreator(int n, uint32_t totalsize, const std::string& filename,
                                                   const std::string& path, LogAppender::ptr log);
                void                    close();

            private:
                const int        mid;
                std::string      mFileName;
                std::string      mPath;
                std::ofstream    mofs;
                uint32_t         mOffset;
                uint32_t         mLength;
                uint32_t         mProgress;
                LogAppender::ptr mlog;
        };
}  // namespace cncd
#endif  // CN_CD_FILEBLOCK_H
