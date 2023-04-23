//
// Created by xSeung on 2023/4/21.
//
#include "connection.h"
namespace cncd {
        void Connection::setState(Connection::State state) {
                m_state = state;
        }
}  // namespace cncd
