//
// Created by xSeung on 2023/4/21.
//
#define _WIN32_WINNT 0x0601
#include "fileblock.h"
#include "filesystem"
#include "fstream"
#include "spdlog/spdlog.h"

namespace mtft {
    FileReader::FileReader(int id, const std::string &fpath, uint64_t offset, uint64_t length)
        : mID(id), mPathWithFileName(fpath), mOffset(offset), mLength(length), mProgress(0) {
        mifs.open(fpath, std::ios::binary);
        if (!mifs.good()) {
            spdlog::error("文件打开失败({})", fpath);
            abort();
        }
        mifs.seekg((int64_t)offset, std::ios::beg);
    }

    bool FileReader::finished() const {
        return mProgress == mLength;
    }

    uint64_t FileReader::read(void *data, uint64_t buffersize) {
        if (this->finished()) {
            spdlog::warn("尝试在已读完的块中继续读出内容");
            return 0;
        }
        uint64_t remain = mLength - mProgress;
        uint64_t size   = remain <= buffersize ? remain : buffersize;
        mifs.read((char *)data, (int64_t)size);
        mProgress += size;
        return size;
    }

    void FileReader::seek(uint64_t progress) {
        mProgress = progress;
        mifs.seekg((int64_t)(mOffset + mProgress), std::ios::beg);
    }

    int FileReader::getID() const {
        return mID;
    }

    auto FileReader::Builder(int n, uint64_t totalsize, const std::string &fpath) -> std::vector<ptr> {
        if (n <= 1) {
            spdlog::error("n<=1");
            abort();
        }
        uint64_t         blocksize = totalsize / (n - 1);
        uint64_t         lastblock = totalsize - (n - 1) * blocksize;
        std::vector<ptr> vec;
        vec.reserve(n - 1);
        for (int i = 0; i < n - 1; i++) {
            vec.emplace_back(std::make_shared<FileReader>(i, fpath, blocksize * i, blocksize));
            spdlog::info("id:{:2}, offset:{:15}, blocksize:{:10}", i, i * blocksize, blocksize);
        }
        vec.emplace_back(std::make_shared<FileReader>(n - 1, fpath, n * blocksize, lastblock));
        spdlog::info("id:{:2}, offset:{:15}, blocksize:{:10}", n - 1, n * blocksize, lastblock);
        return vec;
    }
    FileWriter::FileWriter(int id, const std::string &filename, const std::string &path, uint64_t offset,
                           uint64_t length)
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

    bool FileWriter::finished() const {
        return mProgress == mLength;
    }

    uint64_t FileWriter::write(const void *data, uint64_t buffersize) {
        if (finished()) {
            spdlog::warn("尝试在已写完的块中继续写入内容");
            return 0;
        }
        uint64_t remain = mLength - mProgress;
        uint64_t size   = remain < buffersize ? remain : buffersize;
        mofs.write((char *)data, (int64_t)size);
        mProgress += size;
        return size;
    }

    void FileWriter::seek(uint64_t pos) {
        if (pos > mProgress) {
            return;
        }
        pos       = pos >= 0 ? pos : 0;
        mProgress = pos;
        mofs.seekp((uint32_t)pos, std::ios::beg);
    }

    int FileWriter::getID() const {
        return mid;
    }

    const std::string &FileWriter::getFname() {
        return mFileName;
    }

    uint64_t FileWriter::getProgress() const {
        return mProgress;
    }

    auto FileWriter::Builder(int n, uint64_t totalsize, const std::string &filename, const std::string &path)
        -> std::vector<ptr> {
        if (n <= 1) {
            spdlog::error("n<=1");
            abort();
        }
        std::vector<ptr> vec;
        uint64_t         blocksize = totalsize / (n - 1);
        uint64_t         lastblock = totalsize - (n - 1) * blocksize;
        vec.reserve(n - 1);
        for (int i = 0; i < n - 1; i++) {
            vec.emplace_back(std::make_shared<FileWriter>(i, filename, path, i * blocksize, blocksize));
            spdlog::info("id:{:2}, offset:{:15}, blocksize:{:10}", i, i * blocksize, blocksize);
        }
        vec.emplace_back(std::make_shared<FileWriter>(n - 1, filename, path, n * blocksize, lastblock));
        spdlog::info("id:{:2}, offset:{:15}, blocksize:{:10}", n - 1, n * blocksize, lastblock);
        return vec;
    }
    bool FileWriter::merge(const std::string &fname, const std::vector<std::string> &vec) {
        spdlog::info("创建文件: {}", fname);
        std::ofstream ofs(fname, std::ios::binary);
        if (!ofs.good()) {
            spdlog::error("无法创建文件: {}", fname);
            return false;
        }
        for (auto &&e : vec) {
            auto          temp = std::format("{}{}\\{}", fname, DIR, e);
            std::ifstream _(temp, std::ios::binary);
            if (!_.good()) {
                spdlog::warn("打开文件 {} 失败", temp);
                ofs.close();
                std::filesystem::remove(fname);
                return false;
            }
            spdlog::info("合并文件块: {}", temp);
            ofs << _.rdbuf();
            _.close();
        }
        ofs.close();
        return true;
    }
    void FileWriter::close() {
        mofs.close();
    }
}  // namespace mtft