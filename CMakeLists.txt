cmake_minimum_required(VERSION 3.16)

# -------------------------------- 基础配置 --------------------------------
# 设置应用程序名称，后续所有目标、安装等都用这个变量，方便统一修改
set(APP_NAME My_App)
# 动态获取项目名称（使用源码目录名）
get_filename_component(PROJECT_NAME ${CMAKE_SOURCE_DIR} NAME)
string(TOUPPER ${APP_NAME} PROJECT_NAME_CAPITALIZED)  # 转换为大写（可选）

# -------------------------------- 版本管理 --------------------------------
# 使用 project() 内置版本管理（主版本将被缓存）
project(${APP_NAME}
        VERSION 1.0.0          # 默认版本号
        LANGUAGES CXX          # 指定语言
)

# 允许通过命令行覆盖版本参数
set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "主版本号")
set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "次版本号")
set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "补丁版本号")
set(VERSION_SUFFIX "" CACHE STRING "版本后缀 (如: beta, rc1)")

# 组合完整版本号
if (VERSION_SUFFIX)
    set(PROJECT_FULL_VERSION "${PROJECT_VERSION}-${VERSION_SUFFIX}")
else ()
    set(PROJECT_FULL_VERSION "${PROJECT_VERSION}")
endif ()

# -------------------------------- 构建信息 --------------------------------
# 配置构建信息输出级别
set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)      # 抑制冗余信息
set(CMAKE_SUPPRESS_REGENERATION ON)      # 禁止自动重新生成
set(CMAKE_VERBOSE_MAKEFILE OFF)          # 关闭详细构建日志

# -------------------------------- Git 信息 --------------------------------
find_package(Git QUIET)
set(GIT_HASH "unknown")
set(GIT_BRANCH "unknown")

if (GIT_FOUND)
    # 获取 Git 短哈希
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_HASH_TMP
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_RESULT
    )
    # 获取 Git 分支名
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH_TMP
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE GIT_RESULT
    )

    # 处理执行结果
    if (GIT_RESULT EQUAL 0)
        set(GIT_HASH ${GIT_HASH_TMP})
        set(GIT_BRANCH ${GIT_BRANCH_TMP})
    endif ()
endif ()

# 获取构建时间戳
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# -------------------------------- Qt 配置 --------------------------------
# 启用自动处理 Qt 资源
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找必需 Qt 组件
find_package(Qt6 REQUIRED COMPONENTS
        Core    # 必须显式包含
        Gui     # 必须显式包含
        Widgets
        Sql
)

# -------------------------------- 编译选项 --------------------------------
# C++ 标准配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 目标专属编译选项
add_library(project_warnings INTERFACE
        src/widget/gomoku.cpp
        src/widget/gomoku.h
        src/widget/gomoku.ui
        src/widget/gamewindow.cpp
        src/widget/gamewindow.h
        src/widget/gamewindow.ui)
target_compile_options(project_warnings INTERFACE
        # MSVC 选项
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4          # 警告等级4
        /WX          # 视警告为错误
        /permissive- # 标准一致性模式
        >

        # GCC/Clang 选项
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -Wall
        -Wextra
        -Wpedantic
        >
)

# -------------------------------- 源文件收集 --------------------------------
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 使用 CONFIGURE_DEPENDS 自动跟踪新文件
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        "${PROJECT_ROOT_DIR}/src/*.cpp"
        "${PROJECT_ROOT_DIR}/src/*.h"
        "${PROJECT_ROOT_DIR}/src/*.ui"
        "${PROJECT_ROOT_DIR}/src/*.qrc"   # 建议 qrc 文件放在 src 目录
)

# 在 IDE 中按目录结构组织文件
source_group(TREE ${PROJECT_ROOT_DIR} FILES ${SRC_FILES})

# -------------------------------- 可执行目标 --------------------------------
# 生成可执行文件，所有源文件都打包进 ${APP_NAME}
add_executable(${APP_NAME} ${SRC_FILES}
        src/widget/gomoku.cpp
        src/widget/gomoku.h
        src/widget/gomoku.ui
        src/widget/ui_gomoku.h
        src/widget/gamewindow.cpp
        src/widget/gamewindow.h
        src/widget/gamewindow.ui
        src/widget/chessboard.cpp
        src/widget/chessboard.h
        src/widget/chessboard.ui
        src/widget/gomokuLogic.cpp
        src/widget/gomokuLogic.h
        src/widget/aibrain.cpp
        src/widget/aibrain.h
)

# 链接库和编译器选项
# PRIVATE 表示只对本目标生效
# 必须链接 Qt5::Core/Gui/Widgets/Sql，否则无法编译 Qt 程序
# project_warnings 是自定义的警告设置
# 这里用变量，方便后续统一修改
# 例如：target_link_libraries(${APP_NAME} PRIVATE Qt5::Core ...)
target_link_libraries(${APP_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        project_warnings
)

# 包含目录配置
target_include_directories(${APP_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}      # 自动生成的 moc 文件
)

# -------------------------------- 平台特定配置 --------------------------------
if (WIN32)
    # 定义 Windows 下的宏，减少头文件体积，指定 Windows 10 API
    target_compile_definitions(${APP_NAME} PRIVATE
            WIN32_LEAN_AND_MEAN
            _WIN32_WINNT=0x0A00          # Windows 10
    )
    # Release 模式下为 GUI 程序（无命令行窗口），Debug 下为控制台程序（方便调试）
    set_target_properties(${APP_NAME} PROPERTIES
            WIN32_EXECUTABLE $<CONFIG:Release>
            # 设置可执行文件输出目录为工程根目录下 bin 文件夹，Linux/Windows 都适用
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    )

elseif (APPLE)
    target_compile_definitions(${APP_NAME} PRIVATE MACOS)
    set_target_properties(${APP_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_FULL_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")

else ()
    target_compile_definitions(${APP_NAME} PRIVATE LINUX)
endif ()

# -------------------------------- 构建信息注入 --------------------------------
target_compile_definitions(${APP_NAME} PRIVATE
        BUILD_GIT_HASH="${GIT_HASH}"
        BUILD_GIT_BRANCH="${GIT_BRANCH}"
        BUILD_TIMESTAMP="${BUILD_TIMESTAMP}"
        PROJECT_FULL_VERSION="${PROJECT_FULL_VERSION}"
)

# -------------------------------- 安装规则 --------------------------------
include(GNUInstallDirs)

install(TARGETS ${APP_NAME}
        BUNDLE DESTINATION .                        # macOS .app
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # Windows/Linux 可执行文件
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 共享库
)

# 安装附加资源（假设存在 assets 目录）
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    install(DIRECTORY assets/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/${APP_NAME}
    )
endif ()

# macOS 专用 Bundle 修复
if (APPLE)
    install(CODE "include(BundleUtilities)")
    install(CODE "fixup_bundle(\${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app \"\" \"\")")
endif ()

# -------------------------------- 自动创建项目目录结构 --------------------------------
# 定义目录结构
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(SUBDIRS
        "${SRC_DIR}/resource"
        "${SRC_DIR}/utils"
        "${SRC_DIR}/widget"
)

# 创建目录
foreach (dir ${SUBDIRS})
    if (NOT EXISTS ${dir})
        file(MAKE_DIRECTORY ${dir})
    endif ()
endforeach ()
