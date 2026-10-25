# dataset.neuralnet.cpp — neuralnet.cpp 数据集文件格式 I/O（header-only）
#
# 内容：
#   dataset_format.hpp  文件格式常量/magic/FileLabel（.nndataset / .nnvocab 容器）
#   dataset.hpp         Dataset / TextCorpus / NndatasetBuild 等加载与构建
#
# 依赖：base.neuralnet.cpp（Result/文件 I/O/sha256）+ kvrecord.neuralnet.cpp（容器），
#       经 extern/ 下 submodule。
#
# ⚠ 训练侧的窗口装配(dataset_samples.hpp)不属于本仓——它依赖 algebra/线程池，
#   留在门面仓 neuralnet.cpp。
#
# 消费方式：
#   git submodule add .../{base,kvrecord}.neuralnet.cpp.git extern/{base,kvrecord}
#   add_subdirectory(dataset)
#   target_link_libraries(app PRIVATE neuralnet::dataset)
#   #include <neuralnet.cpp/dataset.hpp>
cmake_minimum_required(VERSION 3.20)
project(neuralnet_dataset LANGUAGES CXX)

# 顺序：先 base（定义 neuralnet_base），再 kvrecord（其内部会检测 neuralnet_base
# 已存在则跳过，避免重复 add_subdirectory 同一目录）。父工程若已定义则都跳过。
if(NOT TARGET neuralnet_base)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/base
                     ${CMAKE_CURRENT_BINARY_DIR}/extern/base)
endif()
if(NOT TARGET neuralnet_kvrecord)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/kvrecord
                     ${CMAKE_CURRENT_BINARY_DIR}/extern/kvrecord)
endif()

add_library(neuralnet_dataset INTERFACE)
add_library(neuralnet::dataset ALIAS neuralnet_dataset)
target_include_directories(neuralnet_dataset INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(neuralnet_dataset INTERFACE neuralnet::base neuralnet::kvrecord)
target_compile_features(neuralnet_dataset INTERFACE cxx_std_23)
