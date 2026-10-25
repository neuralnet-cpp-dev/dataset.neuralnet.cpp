# dataset.neuralnet.cpp

neuralnet.cpp 生态的**数据集文件格式 I/O**库（header-only）。

| 头文件 | 内容 |
|---|---|
| `dataset_format.hpp` | `.nndataset` / `.nnvocab` 容器的 magic / 版本 / `FileLabel` 等格式常量 |
| `dataset.hpp` | `Dataset` / `TextCorpus` / `NndatasetBuild` —— 数据集加载与构建 |

> ⚠ 训练侧的窗口装配（`dataset_samples.hpp`，`TextSampleSet` → batch）依赖 algebra/线程池，**不属于本仓**，留在门面仓 [neuralnet.cpp](https://github.com/neuralnet-cpp-dev/neuralnet.cpp)。

## 依赖

- [`base.neuralnet.cpp`](https://github.com/neuralnet-cpp-dev/base.neuralnet.cpp)（`Result<T>` / 文件 I/O / SHA-256）
- [`kvrecord.neuralnet.cpp`](https://github.com/neuralnet-cpp-dev/kvrecord.neuralnet.cpp)（二进制容器）

均以 submodule 位于 `extern/`。

```bash
git clone --recurse-submodules .../dataset.neuralnet.cpp
```

CMake：

```cmake
add_subdirectory(dataset)   # 自动拉起 extern/{base,kvrecord}
target_link_libraries(app PRIVATE neuralnet::dataset)
```
