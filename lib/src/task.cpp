//
// Created by xSeung on 2023/4/21.
//
#include "task.h"
#include "config.h"
#include "iostream"
namespace cncd {
        using namespace asio;

        Work::Work(LogAppender::ptr ptr, const ip::tcp::endpoint& edp) {
                mlog = ptr;
                medp = edp;
        }
        /// @brief buf ==> json
        /// @param buf 缓存
        /// @param json Json::Value
        /// @return 操作是否成功
        bool Work::ReadJson(streambuf& buf, Json::Value& json) {
                Json::Reader jr;
                auto         size = buf.data().size();
                std::string  str((const char*)buf.data().data(), size);
                json.clear();
                if (jr.parse(str, json)) {
                        buf.consume(size);
                        return true;
                }
                return false;
        }
        /// @brief json ==> buf
        /// @param buf 缓存
        /// @param json Json::Value
        void Work::WriteJson(streambuf& buf, Json::Value& json) {
                auto str  = json.toStyledString();
                auto size = str.size() * sizeof(char);
                json.clear();
                std::memcpy(buf.prepare(JSONSIZE).data(), str.data(), size);
                buf.commit(size);
        }
        UpWork::UpWork(LogAppender::ptr log, const ip::tcp::endpoint& edp) : Work(log, edp) {}

        bool UpWork::uploadFunc(socket_ptr sck) {
                FileReader::ptr freader;
                error_code      ec;
                uint32_t        size;
                uint32_t        progress;
                Json::Value     json;
                // 接受JSON文件
                size = asio::read(*sck.get(), mBuf.prepare(JSONSIZE), ec);
                if (ec) {
                        mlog->log(LogLevel::INFO, LOG(std::format("work({}): {}", freader->getID(), ec.message())));
                        return false;
                }
                // 解析JSON, 更改相应配置
                ReadJson(mBuf, json);
                mid      = json[ID].asInt();
                progress = json[PROGRESS].asInt();
                freader  = mReaders->at(mid);
                freader->seek(progress);
                // 发送文件
                while (!freader->finished() && mBuf.data().size() != 0) {
                        auto size = freader->read((mBuf.prepare(BUFFSIZE).data()), BUFFSIZE);
                        mBuf.commit(size);
                        size = mSckptr->write_some(mBuf.data(), ec);
                        if (ec) {
                                mlog->log(LogLevel::INFO, LOG(std::format("work({}): {}", mid, ec.message())));
                                return false;
                        }
                        mBuf.consume(size);
                }
                // TODO:确认对方全部接收
                return true;
        }

        void UpWork::Func() {
                error_code ec;
                while (true) {
                        auto sck = std::make_shared<ip::tcp::socket>(mioc);
                        sck->connect(medp, ec);
                        if (ec) {
                                mlog->log(LogLevel::INFO, LOG(std::format("work({})创建连接: {}", mid, ec.message())));
                                std::this_thread::sleep_for(std::chrono::seconds(5));
                                continue;
                        }
                        else if (this->uploadFunc(sck)) {
                                break;
                        }
                }
                mlog->log(LogLevel::INFO, LOG(std::format("work({}): 已完成数据发送", mid)));
        }

        DownWork::DownWork(LogAppender::ptr log, const ip::tcp::endpoint& edp) : Work(log, edp) {}

        bool DownWork::downloadFunc() {
                error_code  ec;
                Json::Value json;
                json[ID]       = mFwriter->getID();
                json[PROGRESS] = mFwriter->getProgress();
                WriteJson(mBuf, json);

                return true;
        }

        void DownWork::Func() {}

}  // namespace cncd
