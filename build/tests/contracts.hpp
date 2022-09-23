#pragma once
#include <eosio/testing/tester.hpp>

namespace eosio { namespace testing {

struct contracts {
   static std::vector<uint8_t> system_wasm() { return read_wasm("/root/target/build/tests/../libs/eosio.system/eosio.system.wasm"); }
   static std::vector<char>    system_abi() { return read_abi("/root/target/build/tests/../libs/eosio.system/eosio.system.abi"); }
   
   static std::vector<uint8_t> token_wasm() { return read_wasm("/root/target/build/tests/../libs/eosio.token/eosio.token.wasm"); }
   static std::vector<char>    token_abi() { return read_abi("/root/target/build/tests/../libs/eosio.token/eosio.token.abi"); }

   static std::vector<uint8_t> msig_wasm() { return read_wasm("/root/target/build/tests/../libs/eosio.msig/eosio.msig.wasm"); }
   static std::vector<char>    msig_abi() { return read_abi("/root/target/build/tests/../libs/eosio.msig/eosio.msig.abi"); }

   static std::vector<uint8_t> bios_wasm() { return read_wasm("/root/target/build/tests/../libs/eosio.bios/eosio.bios.wasm"); }
   static std::vector<char>    bios_abi() { return read_abi("/root/target/build/tests/../libs/eosio.bios/eosio.bios.abi"); }

   static std::vector<char>    trail_abi() { return read_abi("/root/target/build/tests/../libs/eosio.trail/eosio.trail.abi"); }
   static std::vector<uint8_t> trail_wasm() { return read_wasm("/root/target/build/tests/../libs/eosio.trail/eosio.trail.wasm"); }

   static std::vector<uint8_t> eosio_tedp_wasm() { return read_wasm("/root/target/build/tests/../eosio.tedp.wasm"); }
   static std::string          eosio_tedp_wast() { return read_wast("/root/target/build/tests/../eosio.tedp.wast"); }
   static std::vector<char>    eosio_tecp_abi() { return read_abi("/root/target/build/tests/../eosio.tedp.abi"); }
   
};
}} //ns eosio::testing
