// ── test_common.hpp — base 子仓测试公共工具（最小版）──────────────────────
//
// 本头是 base.neuralnet.cpp 子仓测试的唯一公共件，**只含 CHECK 宏**，
// 不依赖任何门面（neuralnet.cpp 主仓）头——子仓测试必须能在独立克隆
// （仅本仓 + 其 extern/ submodule）下编译运行。
//
// 约定（与门面仓 src/test_common.hpp 同款）：
//   1. 失败计数器由各测试自带（`int g_fail = 0;` 或 `int g_failures = 0;`）；
//      计数器叫 g_failures 的文件在 include 前加 `#define NN_TEST_COUNTER g_failures`。
//   2. 宏体在使用点展开，聚合器对计数器的重命名照样生效。
// ─────────────────────────────────────────────────────────────────────────

#pragma once

#include <cstdio>

#ifndef NN_TEST_COUNTER
#define NN_TEST_COUNTER g_fail
#endif

#define CHECK(cond, msg)                                                     \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::printf("[FAIL] %s:%d: %s\n", __FILE__, __LINE__, (msg));    \
            ++NN_TEST_COUNTER;                                               \
        }                                                                    \
    } while (0)
