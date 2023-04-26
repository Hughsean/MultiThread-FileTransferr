//
// Created by xSeung on 2023/4/21.
//

#ifndef CN_CD_CONFIG_H
#define CN_CD_CONFIG_H
// const int block_size = 512;
namespace cncd {
        const int      THREADN  = 6;
        const uint32_t BLOCKSIZE = 512;
        enum class State { R, W, F };
}  // namespace cncd

#endif  // CN_CD_CONFIG_H
