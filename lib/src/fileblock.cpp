//
// Created by xSeung on 2023/4/21.
//
#define _WIN32_WINNT 0x0601
#include "fileblock.h"
#include "filesystem"
#include "format"
#include "fstream"
#include "spdlog/spdlog.h"

namespace mtft {
    FileReader::FileReader(int id, const std::string &fpath, uint32_t offset, uint32_t length)
        : mID(id), mPathWithFileName(fpath), mOffset(offset), mLength(length), mProgress(0) {
        mifs.open(fpath, std::ios::binary);
        if (!mifs.good()) {
            spdlog::error("文件打开失败({})", fpath);
            abort();
        }
        mifs.seekg(offset, std::ios::beg);
    }

    bool FileReader::finished() {
        return mProgress == mLength;
    }

    uint32_t FileReader::read(void *data, uint32_t buffersize) {
        if (this->finished()) {
            spdlog::warn("尝试在已读完的块中继续读出内容");
            return 0;
        }
        uint32_t remain = mLength - mProgress;
        uint32_t size   = remain <= buffersize ? remain : buffersize;
        mifs.read((char *)data, size);
        mProgress += size;
        return size;
    }

    void FileReader::seek(uint32_t progress) {
        mProgress = progress;
        mifs.seekg(mOffset + mProgress, std::ios::beg);
    }

    int FileReader::getID() {
        return mID;
    }

    auto FileReader::Builder(int n, uint32_t totalsize, const std::string &fpath) -> std::vector<ptr> {
        if (n <= 1) {
            spdlog::error("n<=1");
            abort();
        }
        // if (n == 1) {
        //     return { std::make_shared<FileReader>(n - 1, fpath, 0, totalsize) };
        // }
        uint32_t         blocksize = totalsize / (n - 1);
        uint32_t         lastblock = totalsize - (n - 1) * blocksize;
        std::vector<ptr> vec;
        vec.reserve(n - 1);
        for (int i = 0; i < n - 1; i++) {
            vec.emplace_back(std::make_shared<FileReader>(i, fpath, blocksize * i, blocksize));
            spdlog::info("id:{:2},offset:{:10},blocksize:{:10}", i, i * blocksize, blocksize);
        }
        vec.emplace_back(std::make_shared<FileReader>(n - 1, fpath, n * blocksize, lastblock));
        spdlog::info("id:{:2},offset:{:10},blocksize:{:10}", n - 1, n * blocksize, lastblock);
        return vec;
    }
    FileWriter::FileWriter(int id, const std::string &filename, const std::string &path, uint32_t offset,
                           uint32_t length)
        : mid(id), mOffset(offset), mLength(length), mPath(path) {
        mProgress        = 0;
        mFileName        = std::format("{}_{}.part", filename, id);
        std::string temp = std::format("{}\\{}", path, mFileName);
        mofs.open(temp, std::ios::binary);
        if (!mofs.good()) {
            spdlog::error("文件打开失败({})", temp);
            abort();
        }
    }

    bool FileWriter::finished() {
        return mProgress == mLength;
    }
    // XXX
    std::atomic_int64_t iii;
    uint32_t            FileWriter::write(const void *data, uint32_t buffersize) {
        if (finished()) {
            spdlog::warn("尝试在已写完的块中继续写入内容");
            return 0;
        }
        uint32_t remain = mLength - mProgress;
        uint32_t size   = remain < buffersize ? remain : buffersize;
        mofs.write((char *)data, size);
        mProgress += size;
        iii++;
        if (iii % 10000 == 0) {
            spdlog::info("write{}", size * 10000);
        }
        return size;
    }

    int FileWriter::getID() {
        return mid;
    }

    const std::string &FileWriter::getFname() {
        return mFileName;
    }

    uint32_t FileWriter::getProgress() {
        return mProgress;
    }

    auto FileWriter::Builder(int n, uint32_t totalsize, const std::string &filename, const std::string &path)
        -> std::vector<ptr> {
        if (n <= 1) {
            spdlog::error("n<=1");
            abort();
        }
        std::vector<ptr> vec;
        uint32_t         blocksize = totalsize / (n - 1);
        uint32_t         lastblock = totalsize - (n - 1) * blocksize;
        vec.reserve(n - 1);
        for (int i = 0; i < n - 1; i++) {
            vec.emplace_back(std::make_shared<FileWriter>(i, filename, path, i * blocksize, blocksize));
            spdlog::info("id:{:2},offset:{:10},blocksize:{:10}", i, i * blocksize, blocksize);
        }
        vec.emplace_back(std::make_shared<FileWriter>(n - 1, filename, path, n * blocksize, lastblock));
        spdlog::info("id:{:2},offset:{:10},blocksize:{:10}", n - 1, n * blocksize, lastblock);
        return vec;
    }
    void FileWriter::merge(const std::string &fname, const std::vector<ptr> &vec) {
        auto str = std::format("{}\\{}", DIR, fname);
        spdlog::info("创建文件: {}", str);
        std::ofstream ofs(str, std::ios::binary);
        if (!ofs.good()) {
            spdlog::warn("{} 合并失败: 无法创建文件", str);
            return;
        }
        for (auto &&e : vec) {
            auto          temp = std::format("{}\\{}", DIR, e->mFileName);
            std::ifstream _(temp, std::ios::binary);
            if (!_.good()) {
                spdlog::warn("打开文件 {} 失败", temp);
                ofs.close();
                std::filesystem::remove(str);
                return;
            }
            spdlog::info("合并文件块: {}", temp);
            ofs << _.rdbuf();
            _.close();
        }
        ofs.close();
    }
    void FileWriter::close() {
        mofs.close();
    }
}  // namespace mtft