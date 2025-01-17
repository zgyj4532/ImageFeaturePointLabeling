# 设置最低的 CMake 版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(ImageFeaturePointLabeling VERSION 1.0)

# 查找 Qt5 库并启用所需模块
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# 查找 Eigen 库（注意：Eigen 不需要编译，只需包含头文件）
find_package(Eigen3 REQUIRED)

# 查找 OpenCV 库
find_package(OpenCV REQUIRED)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 开启 Qt 自动处理的功能（UI、MOC、RCC）
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 指定应用程序的源文件、头文件和 UI 文件
set(SOURCES
    main.cpp
    mainwindow.cpp
    dedistortion.cpp
    
)

set(HEADERS
    mainwindow.h
    dedistortion.h
    common.h
    
)

set(UI_FILES
    mainwindow.ui
    
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

# 链接所需的库（Qt 和 OpenCV）
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${OpenCV_LIBS})

# 包含 OpenCV 头文件路径
include_directories(${OpenCV_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})

# 显示 OpenCV 的头文件路径和库路径
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
message(STATUS "EIGEN_INCLUDE_DIRS: ${EIGEN3_INCLUDE_DIR}")

# 如果使用资源文件（例如图片资源），你需要添加 Qt 的资源文件支持
# set(RCC_FILES resources.qrc)  # 假设你有一个资源文件 resources.qrc
# target_sources(${PROJECT_NAME} PRIVATE ${RCC_FILES})

