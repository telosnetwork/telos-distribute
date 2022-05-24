#pragma once

// Standard.
#include <optional>
#include <vector>

// External
#include <intx/base.hpp>
#include <ecc/uECC.c>
#include <keccak256/k.c>
#include <rlp/rlp.hpp>

// EOS
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>

// Local
#include "constants.hpp"
#include "util.hpp"
#include "exception.hpp"
#include "datastream.hpp"
#include "block.hpp"
#include "transaction.hpp"
#include "context.hpp"
#include "processor.hpp"
#include "program.hpp"
#include "tables.hpp"

namespace eosio_evm
{
  class [[eosio::contract("eosio.evm")]] evm : public eosio::contract
  {
  public:
    using contract::contract;

    evm(eosio::name receiver, eosio::name code, eosio::datastream<const char *> ds)
        : contract(receiver, code, ds),
          _accounts(receiver, receiver.value) {}

    ACTION init(const uint64_t &start_bytes,
        const eosio::asset &start_cost,
        const uint64_t &target_free,
        const uint64_t &min_buy,
        const uint16_t &fee_transfer_pct,
        const uint64_t &gas_per_byte);

    ACTION raw(const eosio::name &ram_payer,
               const std::vector<int8_t> &tx,
               const bool &estimate_gas,
               const std::optional<eosio::checksum160> &sender);

    ACTION create(const eosio::name &account,
                  const std::string &data);

    ACTION openwallet(const eosio::name &account,
                      const eosio::checksum160 &address);

    ACTION withdraw(const eosio::name &to,
                    const eosio::asset &quantity);

    ACTION setresources(const uint64_t &gas_per_byte,
                        const uint64_t &target_free,
                        const uint64_t &min_buy,
                        const uint64_t &fee_transfer_pct);

    ACTION doresources();

    ACTION receipt(const std::vector<int8_t> &tx,
                   // const bigint::checksum256 &hash,
                   const uint32_t &trx_index,
                   const uint64_t &block,
                   // const std::string &trxid,
                   const std::string &from,
                   const uint8_t &status,
                   const uint32_t &epoch,
                   const std::string &createdaddr,
                   const std::string &gasused,
                   const std::string &gasusedblock,
                   const std::string &logs,
                   const std::string &output,
                   const std::string &errors,
                   const std::vector<itx_data> &itxs);

    [[eosio::on_notify("eosio.token::transfer")]] void transfer(const eosio::name &from,
                                                                const eosio::name &to,
                                                                const eosio::asset &quantity,
                                                                const std::string &memo);

    // Extra to match ethereum functionality (calls do not modify state and will always assert)
    ACTION call(
        const eosio::name &ram_payer,
        const std::vector<int8_t> &tx,
        const std::optional<eosio::checksum160> &sender);

    // Action wrappers
    using withdraw_action = eosio::action_wrapper<"withdraw"_n, &evm::withdraw>;
    using transfer_action = eosio::action_wrapper<"transfer"_n, &evm::transfer>;

    // Define account table
    account_table _accounts;

#if (TESTING == true)
    /*
    ACTION teststatetx(const std::vector<int8_t>& tx, const Env& env);
    ACTION devnewstore(const eosio::checksum160& address, const std::string& key, const std::string value);
    ACTION devnewacct(const eosio::checksum160& address, const std::string balance, const std::vector<uint8_t> code, const uint64_t nonce, const eosio::name& account);
    ACTION printstate(const eosio::checksum160& address);
    ACTION printaccount(const eosio::checksum160& address);
    ACTION testtx(const std::vector<int8_t>& tx);
    ACTION printtx(const std::vector<int8_t>& tx);
    */
    ACTION clearall()
    {
      require_auth(get_self());

      account_table db(get_self(), get_self().value);
      auto itr = db.end();
      while (db.begin() != itr)
      {
        itr = db.erase(--itr);
      }

      for (int i = 0; i < 25; i++)
      {
        account_state_table db2(get_self(), i);
        auto itr = db2.end();
        while (db2.begin() != itr)
        {
          itr = db2.erase(--itr);
        }
      }

      receipt_table db3(get_self(), get_self().value);
      auto itr3 = db3.end();
      while (db3.begin() != itr3)
      {
        itr3 = db3.erase(--itr3);
      }

      config_table conf(get_self(), get_self().value);
      if (conf.exists())
      {
        conf.remove();
      }
    }
#endif

  private:
    // EOS Transfer
    void sub_balance(const eosio::name &user, const eosio::asset &quantity);
    void add_balance(const eosio::name &user, const eosio::asset &quantity);
    void add_balance(const eosio::asset &quantity, const eosio::checksum160 &address);
    inline eosio::checksum256 get_trxid()
    {
      auto trxsize = eosio::transaction_size();
      std::vector<char> trxbuf(trxsize);
      uint32_t trxread = eosio::read_transaction(trxbuf.data(), trxsize);
      eosio::check(trxsize == trxread, "read_transaction failed");
      return eosio::sha256(trxbuf.data(), trxsize);
    }
  };
}