﻿//
// Created by xSeung on 2023/4/21.
//
#define _WIN32_WINNT 0x0601
#include "fileblock.h"
#include "fstream"
#include "spdlog/spdlog.h"
// #include "log/log.h"
// #include "format"
// #include "mutex"
// #include "utility"

namespace mtft {
    // namespace log = spdlog;
    // std::mutex    FileBlock::m_mutex{};
    // std::ofstream FileBlock::m_ofs;

    // using namespace asio;
    // FileBlock::FileBlock(int id, Type type, const std::string &&str,
    //                      uint32_t offset, uint32_t length, LogAppender::ptr log)
    //     : m_id(id), m_log(std::move(log)), m_type(type), m_file(str),
    //       m_offset(offset), m_length(length), m_progress(0) {
    //         if (m_type == Type::DOWNLOAD) {
    //                 m_ofs.open(m_file, std::ios::app | std::ios::binary);
    //         }
    //         else {
    //                 m_ifs.open(m_file, std::ios::in | std::ios::binary);
    //                 m_ifs.seekg(m_offset);
    //         }
    // }

    // bool FileBlock::isFinished() const {
    //         return m_progress + 1 == m_length;
    // }

    // uint32_t FileBlock::read(BYTE *data, size_t buffersize) {
    //         if (m_type == Type::DOWNLOAD) {
    //                 m_log->log(
    //                     LogLevel::DEBUG,
    //                     LogEvent::make(__FILE__, __LINE__,
    //                     "写模式不能读文件\n"));
    //                 abort();
    //         }
    //         if (m_length <= buffersize + m_progress) {
    //                 m_log->log(LogLevel::DEBUG,
    //                            LogEvent::make(__FILE__, __LINE__,
    //                            "超出文件块\n"));
    //                 abort();
    //         }
    //         uint32_t remain = m_length - m_progress;
    //         uint32_t size   = remain < buffersize ? remain : buffersize;
    //         m_ifs.read((char *)data, size);
    //         m_progress += size;
    //         return size;
    // }
    // uint32_t FileBlock::write(BYTE *data, size_t buffersize) {
    //         if (m_type == Type::UPLOAD) {
    //                 m_log->log(
    //                     LogLevel::DEBUG,
    //                     LogEvent::make(__FILE__, __LINE__,
    //                     "读模式不能写文件\n"));
    //                 abort();
    //         }
    //         if (m_length <= buffersize + m_progress) {
    //                 m_log->log(LogLevel::DEBUG,
    //                            LogEvent::make(__FILE__, __LINE__,
    //                            "超出文件块\n"));
    //                 abort();
    //         }

    //         std::unique_lock _(m_mutex);
    //         _.lock();

    //         m_ofs.seekp(m_offset + m_progress);
    //         uint32_t remain = m_length - m_progress;
    //         uint32_t size   = remain < buffersize ? remain : buffersize;

    //         m_ofs.write((char *)data, size);
    //         m_progress += size;
    //         return size;
    // }
    // int FileBlock::getID() const {
    //         return m_id;
    // }
    /*--------------------------------------------------------------------------*/
    FileReader::FileReader(int id, const std::string &fpath, uint32_t offset, uint32_t length)
        : mID(id), mPathWithFileName(fpath), mOffset(offset), mLength(length), mProgress(0) {
        mifs.open(fpath, std::ios::binary | std::ios::in);
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
        uint32_t         blocksize = totalsize / (n - 1);
        uint32_t         lastblock = totalsize - (n - 1) * blocksize;
        std::vector<ptr> vec;
        vec.reserve(n - 1);
        for (int i = 0; i < n - 1; i++) {
            vec.emplace_back(std::make_shared<FileReader>(i, fpath, blocksize * i, blocksize));
        }
        vec.emplace_back(std::make_shared<FileReader>(n - 1, fpath, n * blocksize, lastblock));
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

    uint32_t FileWriter::write(const void *data, uint32_t buffersize) {
        if (finished()) {
            spdlog::warn("尝试在已写完的块中继续写入内容");
            return 0;
        }
        uint32_t remain = mLength - mProgress - 1;
        uint32_t size   = remain < buffersize ? remain : buffersize;
        mofs.write((char *)data, size);
        mProgress += size;
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
        }
        vec.emplace_back(std::make_shared<FileWriter>(n - 1, filename, path, n * blocksize, lastblock));
        return vec;
    }
    void FileWriter::close() {
        mofs.close();
    }
}  // namespace mtft