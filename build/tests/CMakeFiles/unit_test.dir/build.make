# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/target/tests

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/target/build/tests

# Include any dependencies generated for this target.
include CMakeFiles/unit_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/unit_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/unit_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/unit_test.dir/flags.make

CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o: CMakeFiles/unit_test.dir/flags.make
CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o: /root/target/tests/eosio.system_tests.cpp
CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o: CMakeFiles/unit_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/target/build/tests/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o -MF CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o.d -o CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o -c /root/target/tests/eosio.system_tests.cpp

CMakeFiles/unit_test.dir/eosio.system_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/unit_test.dir/eosio.system_tests.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/target/tests/eosio.system_tests.cpp > CMakeFiles/unit_test.dir/eosio.system_tests.cpp.i

CMakeFiles/unit_test.dir/eosio.system_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/unit_test.dir/eosio.system_tests.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/target/tests/eosio.system_tests.cpp -o CMakeFiles/unit_test.dir/eosio.system_tests.cpp.s

CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o: CMakeFiles/unit_test.dir/flags.make
CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o: /root/target/tests/eosio.tedp_tests.cpp
CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o: CMakeFiles/unit_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/target/build/tests/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o -MF CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o.d -o CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o -c /root/target/tests/eosio.tedp_tests.cpp

CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/target/tests/eosio.tedp_tests.cpp > CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.i

CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/target/tests/eosio.tedp_tests.cpp -o CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.s

CMakeFiles/unit_test.dir/main.cpp.o: CMakeFiles/unit_test.dir/flags.make
CMakeFiles/unit_test.dir/main.cpp.o: /root/target/tests/main.cpp
CMakeFiles/unit_test.dir/main.cpp.o: CMakeFiles/unit_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/target/build/tests/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/unit_test.dir/main.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/unit_test.dir/main.cpp.o -MF CMakeFiles/unit_test.dir/main.cpp.o.d -o CMakeFiles/unit_test.dir/main.cpp.o -c /root/target/tests/main.cpp

CMakeFiles/unit_test.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/unit_test.dir/main.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/target/tests/main.cpp > CMakeFiles/unit_test.dir/main.cpp.i

CMakeFiles/unit_test.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/unit_test.dir/main.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/target/tests/main.cpp -o CMakeFiles/unit_test.dir/main.cpp.s

# Object files for target unit_test
unit_test_OBJECTS = \
"CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o" \
"CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o" \
"CMakeFiles/unit_test.dir/main.cpp.o"

# External object files for target unit_test
unit_test_EXTERNAL_OBJECTS =

unit_test: CMakeFiles/unit_test.dir/eosio.system_tests.cpp.o
unit_test: CMakeFiles/unit_test.dir/eosio.tedp_tests.cpp.o
unit_test: CMakeFiles/unit_test.dir/main.cpp.o
unit_test: CMakeFiles/unit_test.dir/build.make
unit_test: /root/eosio/1.8/lib/libeosio_testing.a
unit_test: /root/eosio/1.8/lib/libeosio_chain.a
unit_test: /root/eosio/1.8/lib/libfc.a
unit_test: /root/eosio/1.8/lib/libWAST.a
unit_test: /root/eosio/1.8/lib/libWASM.a
unit_test: /root/eosio/1.8/lib/libwabt.a
unit_test: /root/eosio/1.8/lib/libRuntime.a
unit_test: /root/eosio/1.8/lib/libPlatform.a
unit_test: /root/eosio/1.8/lib/libIR.a
unit_test: /root/eosio/1.8/lib/libsoftfloat.a
unit_test: /usr/lib/x86_64-linux-gnu/libcrypto.so
unit_test: /usr/lib/x86_64-linux-gnu/libssl.so
unit_test: /root/eosio/1.8/lib/libLogging.a
unit_test: /root/eosio/1.8/lib/libchainbase.a
unit_test: /root/eosio/1.8/lib/libbuiltins.a
unit_test: /root/eosio/1.8/lib/libsecp256k1.a
unit_test: /usr/lib/x86_64-linux-gnu/libgmp.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86Disassembler.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86AsmParser.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86AsmPrinter.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86CodeGen.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMSelectionDAG.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMDebugInfoDWARF.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMAsmPrinter.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMMCParser.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86Info.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMOrcJIT.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMExecutionEngine.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMCodeGen.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMScalarOpts.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMTransformUtils.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMipo.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMAnalysis.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMTarget.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMMC.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMCore.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMSupport.a
unit_test: /root/eosio/1.8/src/boost_1_70_0/lib/libboost_filesystem.a
unit_test: /root/eosio/1.8/src/boost_1_70_0/lib/libboost_system.a
unit_test: /root/eosio/1.8/src/boost_1_70_0/lib/libboost_chrono.a
unit_test: /root/eosio/1.8/src/boost_1_70_0/lib/libboost_iostreams.a
unit_test: /root/eosio/1.8/src/boost_1_70_0/lib/libboost_date_time.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMDebugInfoCodeView.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMDebugInfoMSF.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86Desc.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86AsmPrinter.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86Info.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMMCDisassembler.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMX86Utils.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMGlobalISel.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMCodeGen.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMTarget.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMRuntimeDyld.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMScalarOpts.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMBitWriter.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMInstCombine.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMIRReader.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMAsmParser.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMInstrumentation.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMLinker.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMVectorize.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMTransformUtils.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMAnalysis.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMObject.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMMCParser.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMMC.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMBitReader.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMProfileData.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMCore.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMSupport.a
unit_test: /usr/lib/llvm-4.0/lib/libLLVMDemangle.a
unit_test: CMakeFiles/unit_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/target/build/tests/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable unit_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/unit_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/unit_test.dir/build: unit_test
.PHONY : CMakeFiles/unit_test.dir/build

CMakeFiles/unit_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/unit_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/unit_test.dir/clean

CMakeFiles/unit_test.dir/depend:
	cd /root/target/build/tests && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/target/tests /root/target/tests /root/target/build/tests /root/target/build/tests /root/target/build/tests/CMakeFiles/unit_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/unit_test.dir/depend

