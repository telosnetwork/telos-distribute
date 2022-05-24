// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright (c) 2020 Syed Jafri. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "system.hpp"
namespace eosio_evm
{
  struct Block
  {
    uint64_t  number     = 0;
    uint64_t  difficulty = 0;
    uint64_t  gas_limit  = 10000000;  // Average on ethereum mainnet
    uint64_t  timestamp  = eosio::current_time_point().sec_since_epoch();
    uint256_t coinbase   = 0;
  };

  inline Block current_block = {};
  inline const Block get_current_block() { return current_block; };

  inline uint256_t get_block_hash(uint64_t block_number) { 
    //eosio::print("Block number: ", block_number, " current block number: ", current_block.number, "\n");
    if (current_block.number == block_number || (current_block.number - block_number) > 256)
      return 0;

    std::array<uint8_t, 32U> keccak_array = keccak_256(to_string((uint256_t) block_number));
    return intx::be::unsafe::load<uint256_t>(keccak_array.data());
  };

  inline void load_current_block () {
    global_state_singleton _system_global(SYSTEM_CONTRACT, SYSTEM_CONTRACT.value);
    auto global = _system_global.get();
    current_block.number = global.block_num;
  }

  #if (TESTING == true)
  struct Env
  {
    std::string currentCoinbase;
    std::string currentDifficulty;
    std::string currentGasLimit;
    std::string currentNumber;
    std::string currentTimestamp;
    std::string previousHash;
  };

  inline void set_current_block (const Env& env) {
    current_block = {
      static_cast<uint64_t>(intx::from_string<uint256_t>(env.currentNumber)),
      static_cast<uint64_t>(intx::from_string<uint256_t>(env.currentDifficulty)),
      static_cast<uint64_t>(intx::from_string<uint256_t>(env.currentGasLimit)),
      static_cast<uint64_t>(intx::from_string<uint256_t>(env.currentTimestamp)),
      intx::from_string<uint256_t>(env.currentCoinbase)
    };
  }
  #endif
} // namespace eosio_evm
