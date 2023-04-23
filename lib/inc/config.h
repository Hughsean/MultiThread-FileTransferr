//
// Created by xSeung on 2023/4/21.
//

#ifndef CN_CD_CONFIG_H
#define CN_CD_CONFIG_H
const int thread_n   = 6;
const int block_size = 512;
namespace cncd {
        enum class Type { UPLOAD, DOWNLOAD };
        enum class State { R, W, F };
}  // namespace cncd

#endif  // CN_CD_CONFIG_H
