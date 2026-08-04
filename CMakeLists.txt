cmake_minimum_required(VERSION 3.15)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(AngelScriptRunner VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

message(STATUS "Fetching AngelScript from GitHub...")
FetchContent_Declare(
    angelscript
    GIT_REPOSITORY https://github.com/anjo76/angelscript.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)

message(STATUS "Fetching doctest from GitHub...")
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.11
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(angelscript doctest)

set(ANGELSCRIPT_SDK_DIR "${angelscript_SOURCE_DIR}/sdk")
set(ANGELSCRIPT_INCLUDE_DIR "${ANGELSCRIPT_SDK_DIR}/angelscript/include")
set(ANGELSCRIPT_ADDON_DIR "${ANGELSCRIPT_SDK_DIR}/add_on")

# Build official AngelScript library target
add_subdirectory("${ANGELSCRIPT_SDK_DIR}/angelscript/projects/cmake" "${CMAKE_BINARY_DIR}/angelscript_lib")

# Gather standard Add-ons sources
set(ANGELSCRIPT_ADDON_SOURCES
    ${ANGELSCRIPT_ADDON_DIR}/scriptbuilder/scriptbuilder.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scriptstdstring/scriptstdstring.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scriptstdstring/scriptstdstring_utils.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scriptarray/scriptarray.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scriptdictionary/scriptdictionary.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scriptmath/scriptmath.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scriptfile/scriptfile.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scriptfile/scriptfilesystem.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scripthandle/scripthandle.cpp
    ${ANGELSCRIPT_ADDON_DIR}/scripthelper/scripthelper.cpp
    ${ANGELSCRIPT_ADDON_DIR}/datetime/datetime.cpp
    ${ANGELSCRIPT_ADDON_DIR}/weakref/weakref.cpp

    # Our addons
    src/addons/ASException.cpp
    src/addons/ASJSON.cpp
    src/addons/ASOptional.cpp
    src/addons/Console.cpp
)

# Application sources
set(APP_SOURCES
    src/main.cpp
    src/addon_registry.cpp
    src/as_predefined.cpp
    src/Tests.cpp
)

add_executable(asrun
    ${ANGELSCRIPT_ADDON_SOURCES}
    ${APP_SOURCES}
)

target_include_directories(asrun PRIVATE
    ${ANGELSCRIPT_INCLUDE_DIR}
    ${ANGELSCRIPT_ADDON_DIR}
    src
    src/utility
    ${doctest_SOURCE_DIR}
)

target_link_libraries(asrun PRIVATE angelscript doctest::doctest)

target_compile_definitions(asrun PRIVATE ASCONSOLE )

if(MSVC)
    target_compile_options(asrun PRIVATE /W4 /EHsc /D_CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(asrun PRIVATE -Wall -Wextra)
endif()
