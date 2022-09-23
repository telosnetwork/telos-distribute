# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/root/target/libs"
  "/root/target/build/libs"
  "/root/target/build/contracts_project-prefix"
  "/root/target/build/contracts_project-prefix/tmp"
  "/root/target/build/contracts_project-prefix/src/contracts_project-stamp"
  "/root/target/build/contracts_project-prefix/src"
  "/root/target/build/contracts_project-prefix/src/contracts_project-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/root/target/build/contracts_project-prefix/src/contracts_project-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/root/target/build/contracts_project-prefix/src/contracts_project-stamp${cfgdir}") # cfgdir has leading slash
endif()
