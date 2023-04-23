//
// Created by xSeung on 2023/4/21.
//
#include "task.h"
#include "config.h"
#include "iostream"
namespace cncd {
        using namespace asio;
        void Worker::uploadFunc() {
                while (!m_fbptr.get()->isFinished()) {
                        try {
                                uint32_t size = m_fbptr.get()->read(m_data, block_size);
                                // m_sckptr.get()->send();
                                m_sckptr.get()->send(buffer(m_data, size));
                        }
                        catch (const system_error& e) {
                                if (e.code() == error::eof) {
                                        m_state = State::W;
                                        
                                        return;
                                }
                        }
                }
                m_state = State::F;
        }
        void Worker::downloadFunc() {
                while (!m_fbptr.get()->isFinished()) {
                        /* code */
                        try {
                                uint32_t size =
                                    m_sckptr.get()->receive(buffer(m_data, block_size));
                        }
                        catch (const system_error& e) {
                                std::cerr << e.what() << '\n';
                        }
                }
        }
        Worker::Worker(Type type, socket_ptr sckptr, FileBlock::ptr fbptr)
            : m_type(type), m_sckptr(sckptr), m_fbptr(fbptr) {
                m_state = State::R;
        }

}  // namespace cncd
