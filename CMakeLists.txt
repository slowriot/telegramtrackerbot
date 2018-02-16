cmake_minimum_required(VERSION 3.1)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

project(TelegramTrackerBot)

add_executable(telegramtrackerbot main.cpp)

target_include_directories(telegramtrackerbot PRIVATE
  include
)
target_link_libraries(telegramtrackerbot PRIVATE
  Threads::Threads
  OpenSSL::SSL
  OpenSSL::Crypto
)
