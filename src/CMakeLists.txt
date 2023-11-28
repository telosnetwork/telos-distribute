cmake_minimum_required(VERSION 3.5)

project(telos_dist)

set(VERSION_MAJOR 3)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
#set(VERSION_SUFFIX develop)

if (VERSION_SUFFIX)
   set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${VERSION_SUFFIX}")
else()
   set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
endif()


find_package(cdt)

message(STATUS "Building telos-distribute v${VERSION_FULL}")

set(CDT_VERSION_MIN "1.6")
set(CDT_VERSION_SOFT_MAX "3.0")

add_contract(
    eosio.tedp
    eosio.tedp
    ${CMAKE_CURRENT_SOURCE_DIR}/eosio.tedp.cpp)

target_include_directories(eosio.tedp
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/eosio.token/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/delphioracle/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/eosio.evm/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../libs/eosio.evm/external
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(eosio.tedp
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
