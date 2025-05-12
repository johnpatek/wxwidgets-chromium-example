cmake_minimum_required(VERSION 3.15)

project(wxwidgets-chromium-example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    wxWidgets
    GIT_REPOSITORY https://github.com/maxtek6/wxWidgets.git
    GIT_TAG feature/chromium-start-request
)
set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
set(wxBUILD_TESTS OFF CACHE BOOL "" FORCE)
set(wxBUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(wxUSE_WEBVIEW_CHROMIUM ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(wxWidgets)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CEF_ROOT}/cmake)
find_package(CEF REQUIRED)

if(WIN32)
    add_executable(example WIN32 main.cpp)
else()
    add_executable(example main.cpp)
endif()
target_include_directories(example PRIVATE ${CEF_ROOT})
target_link_directories(example PRIVATE ${CEF_ROOT}/Release)
target_link_libraries(example PRIVATE wx::core wx::base wx::webview ${CEF_STANDARD_LIBS})