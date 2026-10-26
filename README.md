# dataset.neuralnet.cpp

neuralnet.cpp 生态的**数据集文件格式 I/O**库（header-only）。

| 头文件 | 内容 |
|---|---|
| `dataset_format.hpp` | `.nndataset` / `.nnvocab` 容器的 magic / 版本 / `FileLabel` / `write_nnvocab` / `parse_nnvocab` |
| `dataset.hpp` | `Dataset`（只读加载）/ `TextCorpus` / `TextDoc` / `NndatasetBuild` + `write_nndataset`（构建写入）/ `read_text_docs` / `mark_assistant_span` |

> ⚠ 训练侧的窗口装配（`dataset_samples.hpp`，`TextSampleSet` → batch）依赖 algebra/线程池，**不属于本仓**，留在门面仓 [neuralnet.cpp](https://github.com/neuralnet-cpp-dev/neuralnet.cpp)。

## 文件格式

- **`.nndataset`**：`FileLabel`（magic `NNDS` + u32 版本）+ 根 kvrec（v2，含
  `kind` / `companion`（`vocab` 词表 + `source`（path/sha256/docs）+ `gen`
  （`loss_scope` = `all`|`assistant`））+ `train`/`test` 子集（`num_docs` +
  `doc_index` 绝对偏移表））+ doc 块 arena（`[u64 token 数][tokens…][mask…]`；
  `loss_scope=all` 时无 mask 段，加载时合成全 1）。
- **`.nnvocab`**：`FileLabel`（magic `NNVC`）+ 词表 kvrec。可被 `Dataset::open`
  嗅探打开（词表独立加载场景），但没有数据集子集。
- **存储单位 = doc**（源文本一行 = 一个 doc）；训练单位 = 窗口样本（见上）。
  空 token doc 合法：不产生 token，但**不丢后续 doc_id**（doc_id = 子集内序号 + 1）。

## API 速览

```cpp
#include <neuralnet.cpp/dataset.hpp>

// ── 生成侧 ──────────────────────────────────────────────────
auto docs = nn::read_text_docs("corpus.txt");       // Result<vector<string>>：行 trim + 跳空行
nn::NndatasetBuild b;
b.vocab_kvrec = vocab;                              // 词表 kvrec（本仓不解释内容）
b.source_path = "corpus.txt";
b.source_sha256 = nn::sha256_hex(bytes);
b.source_docs = docs->size();
b.loss_scope = "assistant";                         // "all" | "assistant"
b.train_docs = /* vector<vector<size_t>> 每 doc 的 token 流 */;
b.train_masks = /* 逐 doc 掩码（assistant 模式必需且等长）*/;
std::ofstream ofs("out.nndataset", std::ios::binary);
ofs << nn::write_file_label(nn::NNDATASET_MAGIC, nn::NNDATASET_FILE_VERSION);
auto wr = nn::write_nndataset(ofs, b);              // Result<void>

// ── 加载侧 ──────────────────────────────────────────────────
auto ds = nn::Dataset::open("out.nndataset");       // 嗅探 magic 分派（.nnvocab 也能开）
const nn::DatasetInfo &info = ds->info();           // kind / num_docs / test_docs / loss_scope / source_sha256
auto corpus = ds->load_text(nn::DatasetSubset::Train);   // TextCorpus{token_flow, doc_ids, loss_mask}
auto doc = ds->text_doc(nn::DatasetSubset::Train, 0);    // TextDoc（按 doc 检查/调试）

// ── 词表容器 ────────────────────────────────────────────────
std::string bytes = nn::write_nnvocab(vocab);       // FileLabel + 词表 kvrec(v2)
auto rec = nn::parse_nnvocab(bytes);                // Result<KeyValueRecord>
```

辅助：`nn::mark_assistant_span(tokens, asst_begin, asst_end)` —— 逐 doc 的
`<|assistant|>…<|end_of_assistant|>` 段掩码（与原全局扫描逐位一致）。

## 依赖

- [`base.neuralnet.cpp`](https://github.com/neuralnet-cpp-dev/base.neuralnet.cpp)（`Result<T>` / 文件 I/O / SHA-256）
- [`kvrecord.neuralnet.cpp`](https://github.com/neuralnet-cpp-dev/kvrecord.neuralnet.cpp)（二进制容器）

均以 submodule 位于 `extern/`。

```bash
git clone --recurse-submodules .../dataset.neuralnet.cpp
# 或已 clone 后：git submodule update --init --recursive
```

CMake（库消费）：

```cmake
add_subdirectory(dataset)   # 自动拉起 extern/{base,kvrecord}（若父工程未提供）
target_link_libraries(app PRIVATE neuralnet::dataset)   # 提供 include + C++23
```

## 构建与测试

```bash
git submodule update --init --recursive
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

| 测试 | 覆盖 |
|---|---|
| `dataset_dataset_test` | `read_text_docs` trim 规则（与原实现逐字一致）/ `mark_assistant_span` 与全局扫描逐位一致 / `write_nndataset`→`Dataset::open`→`load_text` 逐位 round-trip（all/assistant 双模式、空 doc 不丢行号、train/test 双子集、元数据）/ `text_doc` 随机访问 / `.nnvocab` 独立打开 / 错误路径（坏 magic/坏版本/masks 不一致） |

> 本仓测试**不经分词器**（token 流手工构造）——BPE 训练 → 并行 tokenize →
> 掩码 → 写读 `.nndataset` 的端到端集成链路在
> [tokenizer.neuralnet.cpp](https://github.com/neuralnet-cpp-dev/tokenizer.neuralnet.cpp)
> 的 `dataset_integration_test`。

被父工程 `add_subdirectory` 消费时测试**默认不构建**（`NN_BUILD_TESTS` 默认值 =
是否顶层工程）；显式 `-DNN_BUILD_TESTS=ON` 可在嵌入时强制打开。

许可证：LGPL-3.0（见 [LICENSE](LICENSE)）。
