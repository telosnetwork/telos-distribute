cmake_minimum_required(VERSION 3.5)

project(telos_dist)

include(ExternalProject)

ExternalProject_Add(
  contract
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/src
  BINARY_DIR ${CMAKE_BINARY_DIR}/src
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_TOOLCHAIN_FILE=${CDT_ROOT}/lib/cmake/cdt/CDTWasmToolchain.cmake
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1)

if (APPLE)
   set(OPENSSL_ROOT "/usr/local/opt/openssl")
elseif (UNIX)
   set(OPENSSL_ROOT "/usr/include/openssl")
endif()
set(SECP256K1_ROOT "/usr/local")

string(REPLACE ";" "|" TEST_FRAMEWORK_PATH "${CMAKE_FRAMEWORK_PATH}")
string(REPLACE ";" "|" TEST_MODULE_PATH "${CMAKE_MODULE_PATH}")

ExternalProject_Add(
  contracts_unit_tests
  LIST_SEPARATOR | # Use the alternate list separator
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${TEST_BUILD_TYPE}
             -DCMAKE_FRAMEWORK_PATH=${TEST_FRAMEWORK_PATH}
            -DCMAKE_MODULE_PATH=${TEST_MODULE_PATH}
            -DCDT_ROOT=${CDT_ROOT}
            -DLLVM_DIR=${LLVM_DIR}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/tests
  BINARY_DIR ${CMAKE_BINARY_DIR}/tests
  BUILD_ALWAYS 1
  TEST_COMMAND   ""
  INSTALL_COMMAND ""
)
