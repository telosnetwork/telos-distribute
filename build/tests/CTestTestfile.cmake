# CMake generated Testfile for 
# Source directory: /root/target/tests
# Build directory: /root/target/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(eosio_system_unit_test "/root/target/build/tests/unit_test" "--run_test=eosio_system_tests" "--report_level=detailed" "--color_output")
set_tests_properties(eosio_system_unit_test PROPERTIES  _BACKTRACE_TRIPLES "/root/target/tests/CMakeLists.txt;39;add_test;/root/target/tests/CMakeLists.txt;0;")
add_test(eosio_tedp_unit_test "/root/target/build/tests/unit_test" "--run_test=eosio_tedp_tests" "--report_level=detailed" "--color_output")
set_tests_properties(eosio_tedp_unit_test PROPERTIES  _BACKTRACE_TRIPLES "/root/target/tests/CMakeLists.txt;39;add_test;/root/target/tests/CMakeLists.txt;0;")
