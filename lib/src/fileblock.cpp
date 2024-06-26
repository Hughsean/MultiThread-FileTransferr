﻿//
// Created by xSeung on 2023/4/21.
//
#define _WIN32_WINNT 0x0601
#include "fileblock.h"
#include "filesystem"
#include "fmt/core.h"
#include "fstream"
#include "spdlog/spdlog.h"

namespace mtft {
    FileReader::FileReader(int id, const std::string &fpath, int64_t offset, int64_t length)
        : mID(id), mPathWithFileName(fpath), mOffset(offset), mLength(length), mProgress(0) {
        mifs.open(fpath, std::ios::binary);
        if (!mifs.good()) {
            spdlog::error("文件打开失败({})", fpath);
            abort();
        }
        mifs.seekg((int64_t)offset, std::ios::beg);
    }

    auto FileReader::finished() const -> bool {
        return mProgress == mLength;
    }

    auto FileReader::read(void *data, int64_t buffersize) -> int64_t {
        if (this->finished()) {
            spdlog::warn("尝试在已读完的块中继续读出内容");
            return 0;
        }
        int64_t remain = mLength - mProgress;
        int64_t size   = remain < buffersize ? remain : buffersize;
        mifs.read((char *)data, size);
        mProgress += size;
        return size;
    }

    void FileReader::seek(int64_t progress) {
        if (progress > mLength || progress < 0) {
            return;
        }
        mProgress = progress;
        mifs.seekg((mOffset + mProgress), std::ios::beg);
    }

    auto FileReader::getID() const -> int {
        return mID;
    }

    void FileReader::close() {
        mifs.close();
    }

    auto FileReader::Build(int n, int64_t totalsize, const std::string &fpath) -> std::vector<ptr> {
        if (n < 1) {
            spdlog::error("n<1");
            abort();
        }
        int64_t          blocksize = totalsize / n;
        int64_t          lastblock = totalsize - (n - 1) * blocksize;
        std::vector<ptr> vec;
        vec.reserve(n);
        for (int i = 0; i < n - 1; i++) {
            vec.emplace_back(std::make_shared<FileReader>(i, fpath, blocksize * i, blocksize));
            spdlog::info("id:{:2}, offset:{:15}, blocksize:{:10}", i, i * blocksize, blocksize);
        }
        vec.emplace_back(
            std::make_shared<FileReader>(n - 1, fpath, (n - 1) * blocksize, lastblock));
        spdlog::info("id:{:2}, offset:{:15}, blocksize:{:10}", n - 1, (n - 1) * blocksize,
                     lastblock);
        return vec;
    }

    FileWriter::FileWriter(int id, const std::string &filename, const std::string &path,
                           int64_t length)
        : mid(id), mLength(length), mProgress(0), mPath(path) {
        mFileName        = fmt::format(FilePartName, filename, id);
        std::string temp = fmt::format("{}\\{}", path, mFileName);
        mofs.open(temp, std::ios::binary);
        if (!mofs.good()) {
            spdlog::error("文件打开失败({})", temp);
            abort();
        }
    }

    auto FileWriter::finished() const -> bool {
        return mProgress == mLength;
    }

    auto FileWriter::write(const void *data, int64_t buffersize) -> int64_t {
        if (finished()) {
            spdlog::warn("尝试在已写完的块中继续写入内容");
            return 0;
        }
        int64_t remain = mLength - mProgress;
        int64_t size   = remain < buffersize ? remain : buffersize;
        mofs.write((char *)data, size);
        mProgress += size;
        return size;
    }

    auto FileWriter::getID() const -> int {
        return mid;
    }

    [[maybe_unused]] auto FileWriter::getFname() -> const std::string & {
        return mFileName;
    }

    auto FileWriter::getProgress() const -> int64_t {
        return mProgress;
    }

    auto FileWriter::merge(const std::string &fname) -> bool {
        spdlog::info("创建文件: {}", fname);
        std::ofstream ofs(fname, std::ios::binary);
        if (!ofs.good()) {
            spdlog::error("无法创建文件: {}", fname);
            return false;
        }
        for (int i = 0; i < THREAD_N; i++) {
            auto          temp = fmt::format("{}{}\\" FilePartName, fname, DIR, fname, i);
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

    auto FileWriter::Build(int n, int64_t totalsize, const std::string &filename,
                           const std::string &path) -> std::vector<ptr> {
        if (n < 1) {
            spdlog::error("n<1");
            abort();
        }
        std::vector<ptr> vec;
        int64_t          blocksize = totalsize / n;
        int64_t          lastblock = totalsize - (n - 1) * blocksize;
        vec.reserve(n);
        for (int i = 0; i < n - 1; i++) {
            vec.emplace_back(std::make_shared<FileWriter>(i, filename, path, blocksize));
            spdlog::info("id:{:2}, blocksize:{:10}", i, blocksize);
        }
        vec.emplace_back(std::make_shared<FileWriter>(n - 1, filename, path, lastblock));
        spdlog::info("id:{:2}, blocksize:{:10}", n - 1, lastblock);
        return vec;
    }
}  // namespace mtft