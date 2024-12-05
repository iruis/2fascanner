cmake_minimum_required(VERSION 3.16)

project(2fascanner VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(RESOURCE_WIN32 "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_subdirectory(zxing-cpp)

qt_standard_project_setup()

qt_add_executable(2fascanner
    MANUAL_FINALIZATION
    base32.cpp
    base32.h
    capturepreview.cpp
    capturepreview.h
    capturepreview.ui
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    otpgenerator.cpp
    otpgenerator.h
    main.cpp

    ${RESOURCE_WIN32}
)

add_dependencies(2fascanner ZXing)

target_link_libraries(2fascanner PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ZXing::ZXing
)

set_target_properties(2fascanner PROPERTIES WIN32_EXECUTABLE ON)

qt_finalize_executable(2fascanner)
