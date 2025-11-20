cmake_minimum_required(VERSION 3.14)
project(laba5_proj
    VERSION 1.0.0
    LANGUAGES CXX
)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка директорий
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIR})

# Создание исполняемого файла
add_executable(laba5_proj_exe main.cpp)

# Настройка компилятора (опционально)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(laba5_proj_exe PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Установка Google Test через URL (без Git)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавление тестов
enable_testing()

# Тестовый исполняемый файл
add_executable(tests test/tests.cpp)

target_link_libraries(tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Добавление тестов в тестовый набор
add_test(NAME DynamicArrayTests COMMAND tests)
