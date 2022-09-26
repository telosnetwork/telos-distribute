// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright (c) 2020 Syed Jafri. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace eosio_evm {
  struct [[eosio::table, eosio::contract("eosio.evm")]] Account {
    uint64_t index;
    eosio::checksum160 address;
    eosio::name account;
    uint64_t nonce;
    std::vector<uint8_t> code;
    bigint::checksum256 balance;

    Account () = default;
    Account (uint256_t _address): address(addressToChecksum160(_address)) {}
    uint64_t primary_key() const { return index; };

    uint64_t get_account_value() const { return account.value; };
    uint256_t get_address() const { return checksum160ToAddress(address); };
    uint256_t get_balance() const { return balance; };
    uint64_t get_nonce() const { return nonce; };
    std::vector<uint8_t> get_code() const { return code; };
    bool is_empty() const { return nonce == 0 && balance == 0 && code.size() == 0; };

    eosio::checksum256 by_address() const { return pad160(address); };


    #if (TESTING == true)
    void print() const {
      eosio::print("\n---Acc Info Start-----");
      eosio::print("\nAddress ", address);
      eosio::print("\nIndex ", index);
      eosio::print("\nEOS Account " + account.to_string());
      eosio::print("\nBalance ", intx::to_string(balance));
      eosio::print("\nCode ", bin2hex(code));
      eosio::print("\nNonce ", nonce);
      eosio::print("\n---Acc Info End---\n");
    }
    #endif /* TESTING */

    EOSLIB_SERIALIZE(Account, (index)(address)(account)(nonce)(code)(balance));
  };

  struct [[eosio::table, eosio::contract("eosio.evm")]] AccountState {
    uint64_t index;
    eosio::checksum256 key;
    bigint::checksum256 value;

    uint64_t primary_key() const { return index; };
    eosio::checksum256 by_key() const { return key; };

    EOSLIB_SERIALIZE(AccountState, (index)(key)(value));
  };

  // TODO: figure out how to get this defined in eosio.evm.hpp
  struct itx_data {
    std::vector<uint8_t> callType;
    eosio::checksum160 from;
    std::vector<uint8_t> gas;
    std::vector<uint8_t> input;
    std::vector<uint8_t> to;
    std::vector<uint8_t> value;
    std::vector<uint8_t> gasUsed;
    std::vector<uint8_t> output;
    uint8_t subtraces;
    std::vector<uint8_t> traceAddress;
    std::string type;
    std::string depth;
    std::map<std::string, std::string> extra;
    void print_json_string() const {

      eosio::print("{\"callType\": \"");
      eosio::printhex(callType.data(), callType.size());
      eosio::print("\",\"from\": \"");
      eosio::print(from);
      eosio::print("\",\"gas\": \"");
      eosio::printhex(gas.data(), gas.size());
      eosio::print("\",\"input\": \"");
      eosio::printhex(input.data(), input.size());
      eosio::print("\",\"to\": \"");
      eosio::printhex(to.data(), to.size());
      eosio::print("\",\"value\": \"");
      eosio::printhex(value.data(), value.size());
      eosio::print("\",\"gasUsed\": \"");
      eosio::printhex(gasUsed.data(), gasUsed.size());
      eosio::print("\",\"output\": \"");
      eosio::printhex(output.data(), output.size());
      eosio::print("\",\"subtraces\": \"");
      eosio::print(std::to_string(subtraces));
      eosio::print("\",\"traceAddress\": [");

        for (auto iter = traceAddress.begin(); iter != traceAddress.end(); )
        {
            eosio::print(*iter);
            if (++iter != traceAddress.end())
                eosio::print(",");
        }

      eosio::print("],\"type\": \"");
      eosio::print(type);
      eosio::print("\",\"depth\": \"");
      eosio::print(depth);
      eosio::print("\",\"extra\": {");
        for (auto iter = extra.begin(); iter != extra.end(); )
        {
          eosio::print("\"", iter->first, "\": \"", iter->second, "\"");
          if (++iter != extra.end())
            eosio::print(",");
        }
      eosio::print("}}");
    }
  };

  struct [[eosio::table, eosio::contract("eosio.evm")]] config {
    uint32_t trx_index = 0;
    uint32_t last_block = 0;
    bigint::checksum256 gas_used_block = 0;
    bigint::checksum256 gas_price = 1;

    EOSLIB_SERIALIZE(config, (trx_index)(last_block)(gas_used_block)(gas_price));
  };

  struct [[eosio::table, eosio::contract("eosio.evm")]] resources {
    bigint::checksum256 gas_per_byte = 0;
    bigint::checksum256 byte_cost = 0;
    uint64_t bytes_used = 0;
    uint64_t bytes_bought = 0;
    uint64_t target_bytes_free = 0;
    uint64_t min_byte_buy = 0;
    bigint::checksum256 fee_balance = 0;
    uint16_t fee_transfer_pct = 0;

    EOSLIB_SERIALIZE(resources, (gas_per_byte)(byte_cost)(bytes_used)(bytes_bought)(target_bytes_free)(min_byte_buy)(fee_balance)(fee_transfer_pct));
  };

  typedef eosio::multi_index<"account"_n, Account,
    eosio::indexed_by<eosio::name("byaddress"), eosio::const_mem_fun<Account, eosio::checksum256, &Account::by_address>>,
    eosio::indexed_by<eosio::name("byaccount"), eosio::const_mem_fun<Account, uint64_t, &Account::get_account_value>>
  > account_table;
  typedef eosio::multi_index<"accountstate"_n, AccountState,
    eosio::indexed_by<eosio::name("bykey"), eosio::const_mem_fun<AccountState, eosio::checksum256, &AccountState::by_key>>
  > account_state_table;

  typedef eosio::singleton<"config"_n, config> config_table;
  typedef eosio::singleton<"resources"_n, resources> resources_table;

}
