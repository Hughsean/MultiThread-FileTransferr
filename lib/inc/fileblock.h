//
// Created by xSeung on 2023/4/21.
//

#ifndef CN_CD_FILEBLOCK_H
#define CN_CD_FILEBLOCK_H

#include "config.h"
#include "fstream"
#include "string"
#include "vector"

namespace mtft {
    class FileReader : Base {
    public:
        using ptr = std::shared_ptr<FileReader>;
        /// @brief          文件块读取构造函数
        /// @param id       块id
        /// @param fpath    文件地址
        /// @param offset   块偏移
        /// @param length   块长度
        FileReader(int id, const std::string& fpath, int64_t offset, int64_t length);
        /// @brief 返回是否读取完毕
        /// @return 读取完成:true;否则:false
        [[nodiscard]] auto finished() const -> bool;
        /// @brief 读取字节流
        /// @param data
        /// @param buffersize
        /// @return
        auto        read(void* data, int64_t buffersize) -> int64_t;
        void        seek(int64_t progress);
        void        close();
        static auto Build(int n, int64_t totalsize, const std::string& fpath) -> std::vector<ptr>;
        [[nodiscard]] auto getID() const -> int;

    private:
        const int     mID;
        std::string   mPathWithFileName;
        std::ifstream mifs;
        int64_t       mOffset;
        int64_t       mLength;
        int64_t       mProgress;
    };
    class FileWriter : Base {
    public:
        using ptr = std::shared_ptr<FileWriter>;
        FileWriter(int id, const std::string& filename, const std::string& path, int64_t length);
        static auto           merge(const std::string& fname) -> bool;
        auto                  write(const void* data, int64_t buffersize) -> int64_t;
        void                  close();
        static auto           Build(int n, int64_t totalsize, const std::string& filename,
                                    const std::string& fpath) -> std::vector<ptr>;
        [[nodiscard]] auto    finished() const -> bool;
        [[nodiscard]] auto    getID() const -> int;
        [[nodiscard]] auto    getProgress() const -> int64_t;
        [[maybe_unused]] auto getFname() -> const std::string&;

    private:
        const int     mid;
        std::string   mFileName;
        std::string   mPath;
        std::ofstream mofs;
        int64_t       mLength;
        int64_t       mProgress;
    };
}  // namespace mtft
#endif
