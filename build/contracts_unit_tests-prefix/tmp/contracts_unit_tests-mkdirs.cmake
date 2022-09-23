# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/root/target/tests"
  "/root/target/build/tests"
  "/root/target/build/contracts_unit_tests-prefix"
  "/root/target/build/contracts_unit_tests-prefix/tmp"
  "/root/target/build/contracts_unit_tests-prefix/src/contracts_unit_tests-stamp"
  "/root/target/build/contracts_unit_tests-prefix/src"
  "/root/target/build/contracts_unit_tests-prefix/src/contracts_unit_tests-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/root/target/build/contracts_unit_tests-prefix/src/contracts_unit_tests-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/root/target/build/contracts_unit_tests-prefix/src/contracts_unit_tests-stamp${cfgdir}") # cfgdir has leading slash
endif()
