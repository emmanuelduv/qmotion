cmake_minimum_required(VERSION 3.0.0)
project(QMotion)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(BUILD_SHARED_LIBS ON)
set(OpenCV_SHARED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(OpenCV REQUIRED)

FILE(GLOB_RECURSE HEADER *.h)
FILE(GLOB_RECURSE SOURCE *.cpp)
FILE(GLOB_RECURSE QRC *.qrc)
FILE(GLOB_RECURSE UI *.ui)
FILE(GLOB_RECURSE RC *.rc)

qt5_add_resources(RESSOURCES ${QRC})

IF(MSVC)
message(${CMAKE_BUILD_TYPE})
    IF(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
        #set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Od")
    ENDIF()

    add_executable(${CMAKE_PROJECT_NAME} WIN32 ${HEADER} ${SOURCE} ${UI} ${RESSOURCES} ${RC})
ELSE()
    add_executable(${CMAKE_PROJECT_NAME} ${HEADER} ${SOURCE} ${UI} ${RESSOURCES})
ENDIF()

MESSAGE(${OpenCV_LIBS})
target_link_libraries(${CMAKE_PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Network ${OpenCV_LIBS})
