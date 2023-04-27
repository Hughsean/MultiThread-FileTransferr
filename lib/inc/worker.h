//
// Created by xSeung on 2023/4/21.
//

#ifndef MAIN_TASK_H
#define MAIN_TASK_H
#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "config.h"
#include "fileblock.h"
#include "fstream"
#include "memory"
#include "set"
#include "thread"
namespace cncd {
        using namespace asio;
        using socket_ptr = std::shared_ptr<ip::tcp::socket>;

        enum class WorkerState { F, W, R };

        class UpWorker {
            public:
                void uploadFunc();
                // void setSocket(socket_ptr);
                UpWorker(socket_ptr sckptr, FileReader fbptr);

            private:
                BYTE        m_data[BLOCKSIZE];
                WorkerState m_state;
                socket_ptr  m_sckptr;
        };

        class DownWorker {
            public:
                void downloadFunc();
                DownWorker();

            private:
        };

}  // namespace cncd
#endif  // MAIN_TASK_H
