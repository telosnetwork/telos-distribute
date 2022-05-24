// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright (c) 2020 Syed Jafri. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include <optional>

#include "constants.hpp"
#include "util.hpp"
#include "tables.hpp"

namespace eosio_evm
{
  /**
   * Transaction Helpers
   */
  inline bool is_pre_eip_155(size_t v) { return v == 27 || v == 28; }

  inline size_t from_ethereum_recovery_id (size_t v)
  {
    if (is_pre_eip_155(v)) {
      return v - PRE_155_V_START;
    }

    constexpr auto min_valid_v = 37u;
    eosio::check(v >= min_valid_v, "Invalid Transaction: Expected v to encode a valid chain ID");

    const size_t rec_id = (v - POST_155_V_START) % 2;
    const size_t chain_id = ((v - rec_id) - POST_155_V_START) / 2;
    eosio::check(chain_id == CURRENT_CHAIN_ID, "Invalid Transaction: Chain ID mismatch");

    return rec_id;
  }

  /**
   * EVM Logs
   */
  struct LogEntry
  {
    Address address;
    std::vector<uint8_t> data;
    std::vector<uint256_t> topics;

    #if (PRINT_LOGS == true)
    void print_topics() const {
      eosio::print("[");
      for (auto i = 0; i < topics.size(); i++) {
        eosio::print("\"");

        std::vector<uint8_t> topic_bytes = intx::to_byte_string(topics[i]);
        eosio::printhex(topic_bytes.data(), topic_bytes.size());

        if (i < topics.size() - 1) {
          eosio::print("\",");
        } else {
          eosio::print("\"");
        }
      }
      eosio::print("]");
    }
    #endif /** PRINT_LOGS **/
  };

  struct LogHandler
  {
    std::vector<LogEntry> logs = {};
    inline void add(LogEntry&& e) { logs.emplace_back(e); }
    inline void pop() { if(!logs.empty()) { logs.pop_back(); } }

    #if (PRINT_LOGS == true)
    void print_json_string() const {
      eosio::print("[");

      for (auto i = 0; i < logs.size(); i++) {

        eosio::print("{\"address\": \"");
        std::vector<uint8_t> address_bytes = intx::to_byte_string(logs[i].address);
        eosio::printhex(address_bytes.data(), address_bytes.size());

        eosio::print("\",\"data\": \"");
        eosio::printhex(logs[i].data.data(), logs[i].data.size());

        eosio::print("\",\"topics\": ");
        logs[i].print_topics();

        if (i < logs.size() - 1) {
          eosio::print("},");
        } else {
            eosio::print("}");
        }
      }
      eosio::print("]");
    }
    #endif /** PRINT_LOGS **/
  };

  /**
   * State modifications: Stored for Reverting
   */
  enum class StateModificationType {
    NONE = 0,
    STORE_KV,
    CREATE_ACCOUNT,
    SET_CODE,
    INCREMENT_NONCE,
    ORIGINAL_STORAGE,
    TRANSFER,
    LOG,
    SELF_DESTRUCT,
    ACCESS_LIST_ADDRESS,
    ACCESS_LIST_SLOT
  };
  using SMT = StateModificationType;

  struct StateModification {
    StateModificationType type;
    uint64_t index;
    uint256_t key;
    uint256_t oldvalue;
    uint256_t amount;
    uint256_t newvalue;

    #if (DEBUG_2929)
    // #if (TESTING == true)
    void print_state_mod() const {
      std::map<StateModificationType, std::string> reverseState = {
        { SMT::STORE_KV, "STORE_KV"   },
        { SMT::CREATE_ACCOUNT, "CREATE_ACCOUNT"    },
        { SMT::SET_CODE, "SET_CODE"    },
        { SMT::INCREMENT_NONCE, "INCREMENT_NONCE"    },
        { SMT::ORIGINAL_STORAGE, "ORIGINAL_STORAGE"    },
        { SMT::TRANSFER, "TRANSFER"    },
        { SMT::LOG, "LOG"    },
        { SMT::SELF_DESTRUCT, "SELF_DESTRUCT"    },
        { SMT::ACCESS_LIST_ADDRESS, "ACCESS_LIST_ADDRESS"    },
        { SMT::ACCESS_LIST_SLOT, "ACCESS_LIST_SLOT"    }
      };

      eosio::print("\nType: ", reverseState[type], ", Index: ", index, ", Key: ", intx::to_string(key), " OldValue: ", intx::to_string(oldvalue), " NewValue: ", intx::to_string(newvalue), " Amount: ", intx::to_string(amount));
    }
    #endif /** Testing **/
  };

  struct EthereumTransaction {
    uint256_t nonce;           // A scalar value equal to the number of transactions sent by the sender;
    uint32_t transaction_index;// The index of this transaction in the block
    uint256_t gas_price;       // A scalar value equal to the number of Wei to be paid per unit of gas for all computation costs incurred as a result of the execution of this transaction;
    uint256_t charged_gas_price; // The charged gas price based on the config table, if greater than gas_price the transaction will fail
    uint256_t gas_limit;       // A scalar value equal to the maximum amount of gas that should be used in executing this transaction
    // uint256_t min_gas_cost;    // The minimum cost of the transaction
    std::vector<uint8_t> to;   // Currently set as vector of bytes.
    uint256_t value;           // A scalar value equal to the number of Wei to be transferred to the message call’s recipient or, in the case of contract creation, as an endowment to the newly created account; forma
    std::vector<uint8_t> data; // An unlimited size byte array specifying the input data of the message call
    eosio::name self;          // The contract name
    std::optional<eosio::checksum160> sender; // Address recovered from 1) signature or 2) EOSIO Account Table (authorized by EOSIO account in case 2)
    std::optional<Address> to_address;        // Currently set as 256 bit. The 160-bit address of the message call’s recipient or, for a contract creation transaction, ∅, used here to denote the only member of B0 ;
    std::optional<Address> created_address;   // If create transaction, the address that was created
    std::unique_ptr<Account> sender_account;  // Pointer to sender account
    std::vector<Address> selfdestruct_list;   // SELFDESTRUCT List
    LogHandler logs = {};                     // Log handler for transaction
    eosio::checksum256 hash = {};             // Hash of transaction
    eosio::checksum256 trxid = {};            // eosio transaction id
    std::vector<std::string> errors;          // Keeps track of errors

    // EIP-2929 accessed addresses and storage keys
    std::map<Address, std::vector<uint256_t>> access_list;

    // Internal transactions info
    std::vector<itx_data> internal_txs;       // Internal transactions list
    std::vector<uint8_t> tx_traceAddress;
    std::vector<uint8_t> tx_traceAddress_out;
    std::vector<uint8_t> subtraces_counter = {0};
    size_t prev_depth = 0;

    uint256_t gas_used;       // Gas used in transaction
    uint256_t gas_refunds;    // Refunds processed in transaction
    uint64_t rambytes_used = 0;  // Bytes added in transaction
    //uint64_t rambytes_cost;  // Bytes added in transaction
    std::vector<StateModification> state_modifications;                  // State modifications
    std::map<uint64_t, std::map<uint256_t, uint256_t>> original_storage; // Cache for SSTORE

    // Signature data
    uint8_t v;   // Recovery ID
    uint256_t r; // Output
    uint256_t s; // Output

    // Ram Payer
    eosio::name ram_payer;
    // bool ram_as_gas;

    // RLP constructor
    EthereumTransaction(const std::vector<int8_t>& encoded, eosio::name ram_payer_account, const eosio::name& contract_self, eosio::checksum256 telos_trxid)
    {
      if (DEBUG_2929) eosio::print("\n---------------- New EthereumTransaction ----------------\n");
      // Max Transaction size
      eosio::check(encoded.size() < MAX_TX_SIZE, "Invalid Transaction: Max size of a transaction is 128 KB");
      eosio::check(encoded.size() > 0, "Invalid Transaction: No encoded data");

      self = contract_self;

      // Decode
      auto rlp = rlp::decode(encoded);
      auto values = rlp.values;

      nonce     = from_big_endian(&values[0].value[0], values[0].value.size());
      gas_price = from_big_endian(&values[1].value[0], values[1].value.size());
      gas_limit = from_big_endian(&values[2].value[0], values[2].value.size());
      to        = values[3].value;
      value     = from_big_endian(&values[4].value[0], values[4].value.size());
      data      = values[5].value;
      v         = values.size() > 6 && values[6].value.size() > 0 ? values[6].value[0] : 0;
      r         = values.size() > 7 && values[7].value.size() > 0 ? from_big_endian(&values[7].value[0], values[7].value.size()) : 0;
      s         = values.size() > 8 && values[8].value.size() > 0 ? from_big_endian(&values[8].value[0], values[8].value.size()) : 0;

      // Validate To Address
      eosio::check(to.empty() || to.size() == 20, "Invalid Transaction: to address must be 40 characters long if provided (excluding 0x prefix)");
      if (!to.empty()) {
        to_address = from_big_endian(to.data(), to.size());
      }

      // Validate Value
      eosio::check(value >= 0, "Invalid Transaction: Value cannot be negative.");

      // Hash
      hash = keccak_256(encode());

      // Telos transaction id
      trxid = telos_trxid;

      // RAM Payer
      // ram_as_gas = contract_self == ram_payer_account;

      //if (!ram_as_gas)
      if (contract_self != ram_payer_account)
        eosio::require_auth(ram_payer_account);

      ram_payer = ram_payer_account;

      // Gas
      initialize_base_gas();

      // Block
      load_current_block();

      // stores transaction_index in config and sets ram price
      config_setup();
    }
    ~EthereumTransaction() = default;

    bool is_zero() { return !r && !s; }
    bool is_create() const { return !to_address.has_value(); }
    uint256_t gas_left() const { return gas_limit - gas_used; }
    std::string encode() const { return rlp::encode(nonce, gas_price, gas_limit, to, value, data, v, r, s); }

    void initialize_base_gas () {
      gas_used = GP_TRANSACTION;

      for (auto& i: data) {
        gas_used += i == 0
          ? GP_TXDATAZERO
          : GP_TXDATANONZERO;
      }

      if (is_create()) {
        gas_used += GP_TXCREATE;
      }

      if (gas_used > gas_limit)
          eosio::check(false, "Gas limit " + to_string(gas_limit) + " is too low for initialization, minimum " + to_string(gas_used) + " required.");
    }

    void config_setup() {
      config_table conf(self, self.value);
      auto c = conf.get();
      uint64_t current_block = get_current_block().number;

      if (c.gas_price > gas_price) {
          if (eosio::has_auth(eosio::name("eosio"))) {
              // eosio can bypass the configured gas price, for keyless deployments like ERC1820 and Gnosis safe
              //  "Nick's method" - https://eips.ethereum.org/EIPS/eip-1820
              charged_gas_price = gas_price;
          } else {
              // fail if the transaction gas price is less than the configured minimum, we won't spend more than the transaction was signed for
              eosio::check(false, "Transaction gas price " + to_string(gas_price) +
                                  " is less than the current fixed gas price of " + to_string((uint256_t) c.gas_price));
          }
      } else {
          // set the gas_price to the minimum
          charged_gas_price = c.gas_price;
      }

      if (current_block != c.last_block) {
        c.last_block = current_block;
        c.gas_used_block = 0;
        c.trx_index = 0;
      } else {
        c.trx_index++;
      }

      transaction_index = c.trx_index;
      conf.set(c, self);
    }

    void to_recoverable_signature(std::array<uint8_t, 65>& sig) const
    {
      // V
      uint8_t recovery_id = from_ethereum_recovery_id(v);
      sig[0] = recovery_id + 27 + 4; // + 4 as it is compressed

      // R and S valdiation
      eosio::check(s >= 1 && s <= intx::from_string<uint256_t>("0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5D576E7357A4501DDFE92F46681B20A0"), "Invalid Transaction: s value in signature must be between 1 and 0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5D576E7357A4501DDFE92F46681B20A0, inclusive.");

      // R and S
      intx::be::unsafe::store(sig.data() + 1, r);
      intx::be::unsafe::store(sig.data() + 1 + R_FIXED_LENGTH, s);
    }

    const KeccakHash to_be_signed()
    {
      if (is_pre_eip_155(v))
      {
        return keccak_256(rlp::encode(nonce, gas_price, gas_limit, to, value, data));
      }
      else
      {
        return keccak_256(rlp::encode( nonce, gas_price, gas_limit, to, value, data, CURRENT_CHAIN_ID, 0, 0 ));
      }
    }

    eosio::checksum160 get_sender()
    {
      // Return early if already exists
      if (sender.has_value()) {
        return *sender;
      }

      // Get transaction hash as checksum256 message
      const std::array<uint8_t, 32u> message = to_be_signed();
      eosio::checksum256 messageChecksum = eosio::fixed_bytes<32>(message);

      // Get recoverable signature
      std::array<uint8_t, 65> arr;
      to_recoverable_signature(arr);

      eosio::ecc_signature ecc_sig;
      std::memcpy(ecc_sig.data(), &arr, sizeof(arr));
      eosio::signature signature = eosio::signature { std::in_place_index<0>, ecc_sig };

      // Recover
      eosio::public_key recovered_variant = eosio::recover_key(messageChecksum, signature);
      eosio::ecc_public_key compressed_public_key = std::get<0>(recovered_variant);
      std::vector<uint8_t> proper_compressed_key( std::begin(compressed_public_key), std::end(compressed_public_key) );

      // Decompress the ETH pubkey
      uint8_t public_key[65];
      public_key[0] = MBEDTLS_ASN1_OCTET_STRING; // 04 prefix
      uECC_decompress(proper_compressed_key.data(), public_key + 1, uECC_secp256k1());

      // Hash key to get address
      std::array<uint8_t, 32u> hashed_key = keccak_256(public_key + 1, 64);

      // Set sender
      sender = toChecksum160(hashed_key);

      return *sender;
    }

    uint256_t from_big_endian(const uint8_t* begin, size_t size = 32u)
    {
      // Size validation
      eosio::check(size <= 32, "Invalid Transaction: Calling from_big_endian with oversized array");

      if (size == 32)
      {
        return intx::be::unsafe::load<uint256_t>(begin);
      }
      else
      {
        uint8_t tmp[32] = {};
        const auto offset = 32 - size;
        if ( size ) { memcpy(tmp + offset, begin, size); }        

        return intx::be::load<uint256_t>(tmp);
      }
    }

    inline void first_storage_emplaced() {
      rambytes_used += 457;
    }
    inline void first_storage_erased() {
      rambytes_used -= 457;
    }
    inline void storage_emplaced() {
      rambytes_used += 347;
    }
    inline void storage_erased() {
      rambytes_used -= 347;
    }
    inline void account_emplaced() {
      rambytes_used += 478; // likely 481
    }
    inline void code_modified(int64_t bytes) {
      rambytes_used += bytes+1;
    }
    inline void code_removed(int64_t bytes) {
      rambytes_used -= bytes+1;
    }

    /**
     * State Modifications and original storage
     */
    inline void add_modification(const StateModification& modification) {
      // #if (DEBUG_2929)
      //   eosio::print("\nDEBUG 2929: Add modification to state: ");
      //   modification.print_state_mod();
      // #endif
      state_modifications.emplace_back(modification);
    }
    inline uint256_t find_original(const uint64_t& address_index, const uint256_t& key) {
      if (original_storage.count(address_index) == 0 || original_storage[address_index].count(key) == 0) {
        return 0;
      } else {
        return original_storage[address_index][key];
      }
    }
    inline void emplace_original(const uint64_t& address_index, const uint256_t& key, const uint256_t& value) {
      if (original_storage.count(address_index) == 0 || original_storage[address_index].count(key) == 0) {
        original_storage[address_index][key] = value;
      }
    }

    void print_errors_as_json_string() const {
      eosio::print("[");
      std::string firstErr = errors.size() > 0 ? errors[0] : "";
      bool haveError = false;
      for (auto i = 0; i < errors.size(); i++) {
        // we seem to have duplicate errors, let's not persist them
        if (i != 0 && errors[i] == firstErr)
            continue;

        if (haveError)
            eosio::print(",");

        haveError = true;
        eosio::print("\"", errors[i], "\"");
      }
      eosio::print("]");
    }

    void increment_block_gas() const {
      config_table conf(self, self.value);
      auto c = conf.get();
      c.gas_used_block += gas_used;
      conf.set(c, self);
    }

    void print_receipt(const ExecResult& result) const
    {
      config_table conf(self, self.value);
      auto c = conf.get();
      uint8_t status = result.er == ExitReason::returned ? 1 : 0;

      eosio::print("RCPT{{{\"charged_gas\": \"");
      std::vector<uint8_t> gp_bytes = intx::to_byte_string(charged_gas_price);
      eosio::printhex(gp_bytes.data(), gp_bytes.size());
      eosio::print("\","),

      eosio::print("\"trx_index\": ", transaction_index,
                   ",\"block\": ", get_current_block().number,
                   ",\"status\": ", status,
                   ",\"epoch\": ", eosio::current_time_point().sec_since_epoch(),
                   ",\"createdaddr\": \"");
        if (created_address) {
            std::vector<uint8_t> created_bytes = intx::to_byte_string(*created_address);
            eosio::printhex(created_bytes.data(), created_bytes.size());
        }

        eosio::print("\",\"gasused\": \"");
        std::vector<uint8_t> gas_used_bytes = intx::to_byte_string(gas_used);
        eosio::printhex(gas_used_bytes.data(), gas_used_bytes.size());

        eosio::print("\",\"gasusedblock\": \"");
        std::vector<uint8_t> gas_used_block_bytes = intx::to_byte_string(c.gas_used_block);
        eosio::printhex(gas_used_block_bytes.data(), gas_used_block_bytes.size());

        eosio::print("\",\"logs\": ");
        logs.print_json_string();

        eosio::print(",\"output\": \"");
        eosio::printhex(result.output.data(), result.output.size());

        eosio::print("\",\"errors\": ");
        print_errors_as_json_string();

        eosio::print(",\"itxs\": [");
        for (auto iter = internal_txs.begin(); iter != internal_txs.end(); )
        {
            iter->print_json_string();
            if (++iter != internal_txs.end())
                eosio::print(",");
        }
        eosio::print("]}}}RCPT\n");
    }

    #if (TESTING == true)
    void printhex() const
    {
      eosio::print(
        // "data ",      bin2hex(data),        "\n",
        "gasLimit ",  intx::hex(gas_limit), "\n",
        "gasPrice ",  intx::hex(gas_price), "\n",
        "data ",      bin2hex(data),        "\n",
        "nonce ",     intx::hex(nonce),     "\n",
        "to ",        bin2hex(to),          "\n",
        "value ",     intx::hex(value),     "\n",
        "v ",         v,                    "\n"
        "r ",         intx::hex(r),         "\n",
        "s ",         intx::hex(s),         "\n\n",
        "sender ",    *sender,              "\n",
        "hash ",      hash,                 "\n"
      );
    }
    #endif /* TESTING */
  };
} // namespace eosio_evm
