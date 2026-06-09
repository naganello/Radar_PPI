cmake_minimum_required(VERSION 3.16)

project(Radar_ppi VERSION 0.4.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(Qt6_DIR C:/Qt6/qt6-build/lib/cmake/Qt6)
find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets OpenGL OpenGLWidgets Xml)

set(ASTERIXQT_HEADERS
    include/AsterixQt/AsterixQtGlobal.h
    include/AsterixQt/AsterixTypes.h
    include/AsterixQt/ThreadSafeQueue.h
    include/AsterixQt/UdpMulticastReceiver.h
    include/AsterixQt/Cat048Decoder.h
    include/AsterixQt/Cat240Decoder.h
    include/AsterixQt/AsterixDecoderWorker.h
    include/AsterixQt/AsterixService.h
)

set(ASTERIXQT_SOURCES
    src/AsterixQt/UdpMulticastReceiver.cpp
    src/AsterixQt/Cat048Decoder.cpp
    src/AsterixQt/Cat240Decoder.cpp
    src/AsterixQt/AsterixDecoderWorker.cpp
    src/AsterixQt/AsterixService.cpp
)

add_library(AsterixQt STATIC
    ${ASTERIXQT_HEADERS}
    ${ASTERIXQT_SOURCES}
)

add_library(AsterixQt::AsterixQt ALIAS AsterixQt)

set_target_properties(AsterixQt PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(AsterixQt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(AsterixQt PUBLIC ASTERIXQT_STATIC)

target_link_libraries(AsterixQt
    PUBLIC
        Qt6::Core
        Qt6::Network
)

add_executable(Radar_ppi
    src/Radar_ppi/main.cpp
    src/Radar_ppi/MainWindow.h
    src/Radar_ppi/MainWindow.cpp
    src/Radar_ppi/PpiWidget.h
    src/Radar_ppi/PpiWidget.cpp
    src/Radar_ppi/MulticastConfigDialog.h
    src/Radar_ppi/MulticastConfigDialog.cpp
)

set_target_properties(Radar_ppi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(Radar_ppi
    PRIVATE
        AsterixQt::AsterixQt
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Qt6::Xml
)
