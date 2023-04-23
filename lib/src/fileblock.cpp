//
// Created by xSeung on 2023/4/21.
//
#define _WIN32_WINNT 0x0601

#include "fileblock.h"
#include "mutex"
#include "utility"

namespace cncd {
        std::mutex    FileBlock::m_mutex{};
        std::ofstream FileBlock::m_ofs;

        using namespace asio;
        FileBlock::FileBlock(int id, Type type, const std::string &&str,
                             uint32_t offset, uint32_t length, LogAppender::ptr log)
            : m_id(id), m_log(std::move(log)), m_type(type), m_file(str),
              m_offset(offset), m_length(length), m_progress(0) {
                if (m_type == Type::DOWNLOAD) {
                        m_ofs.open(m_file, std::ios::app | std::ios::binary);
                }
                else {
                        m_ifs.open(m_file, std::ios::in | std::ios::binary);
                        m_ifs.seekg(m_offset);
                }
        }

        bool FileBlock::isFinished() const {
                return m_progress + 1 == m_length;
        }

        uint32_t FileBlock::read(BYTE *data, size_t buffersize) {
                if (m_type == Type::DOWNLOAD) {
                        m_log->log(
                            LogLevel::DEBUG,
                            LogEvent::make(__FILE__, __LINE__, "写模式不能读文件\n"));
                        abort();
                }
                if (m_length <= buffersize + m_progress) {
                        m_log->log(LogLevel::DEBUG,
                                   LogEvent::make(__FILE__, __LINE__, "超出文件块\n"));
                        abort();
                }
                uint32_t remain = m_length - m_progress;
                uint32_t size   = remain < buffersize ? remain : buffersize;
                m_ifs.read((char *)data, size);
                m_progress += size;
                return size;
        }
        uint32_t FileBlock::write(BYTE *data, size_t buffersize) {
                if (m_type == Type::UPLOAD) {
                        m_log->log(
                            LogLevel::DEBUG,
                            LogEvent::make(__FILE__, __LINE__, "读模式不能写文件\n"));
                        abort();
                }
                if (m_length <= buffersize + m_progress) {
                        m_log->log(LogLevel::DEBUG,
                                   LogEvent::make(__FILE__, __LINE__, "超出文件块\n"));
                        abort();
                }

                std::unique_lock _(m_mutex);
                _.lock();

                m_ofs.seekp(m_offset + m_progress);
                uint32_t remain = m_length - m_progress;
                uint32_t size   = remain < buffersize ? remain : buffersize;

                m_ofs.write((char *)data, size);
                m_progress += size;
                return size;
        }
        int FileBlock::getID() const {
                return m_id;
        }

        FileReader::FileReader(int id, const std::string &&fname, uint32_t offset,
                               uint32_t length, LogAppender::ptr log)
            : m_id(id), m_file_name(fname), m_offect(offset), m_length(length),
              m_log(log), m_progress(0) {
                m_ifs.open(fname, std::ios::binary);
                if (m_ifs.good()) {
                        m_ifs.seekg(offset, std::ios::beg);
                }
                else {
                        m_log.get()->log(
                            LogLevel::INFO,
                            LogEvent::make(__FILE__, __LINE__, "文件打开失败\n"));
                        abort();
                }
        }

        bool FileReader::finished() {
                return m_progress + 1 == m_length;
        }

        uint32_t FileReader::read(BYTE *data, uint32_t buffersize) {
                if (this->finished()) {
                        m_log.get()->log(
                            LogLevel::DEBUG,
                            LogEvent::make(__FILE__, __LINE__, "文件块超出范围\n"));
                        abort();
                }
                uint32_t remain = m_length - m_progress - 1;
                uint32_t size   = remain <= buffersize ? remain : buffersize;
                m_ifs.readsome((char *)data, size);
                return size;
        }

        int FileReader::getID() {
                return m_id;
        }

}  // namespace cncd
