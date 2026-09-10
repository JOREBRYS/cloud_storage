cmake_minimum_required(VERSION 3.10)
project(cloud_storage)

find_package(Boost REQUIRED COMPONENTS system)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(cloud_storage
    src/Main.cpp
    src/Server.cpp
    src/Session.cpp
    src/DatabaseAccessor.cpp
    src/FileManager.cpp)

target_link_libraries(cloud_storage Boost::system sqlite3 OpenSSL::SSL)
target_compile_features(cloud_storage PRIVATE cxx_std_17)