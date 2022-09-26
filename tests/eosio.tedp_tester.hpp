#pragma once

#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/resource_limits.hpp>
#include "contracts.hpp"
#include "test_symbol.hpp"
#include "eosio.system_tester.hpp"

#include <fc/variant_object.hpp>
#include <fstream>

using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

#ifndef TESTER
#ifdef NON_VALIDATING_TEST
#define TESTER tester
#else
#define TESTER validating_tester
#endif
#endif

class eosio_tedp_tester : public eosio_system::eosio_system_tester { 
public:
    abi_serializer tedp_abi_ser;

    const name test_account = name("exrsrv.tf");

    eosio_tedp_tester() {
        produce_blocks( 2 );
        create_accounts_with_resources({ "tf"_n, "econdevfunds"_n, "eosio.evm"_n });
        set_code( test_account, contracts::eosio_tedp_wasm() );
        set_abi( test_account, contracts::eosio_tecp_abi().data() );
        {
            const auto& accnt = control->db().get<account_object, by_name>(test_account);
            abi_def abi;
            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
            tedp_abi_ser.set_abi(abi, abi_serializer_max_time);
        }
    }

    asset get_balance( const account_name& act, symbol balance_symbol = symbol{CORE_SYM}, const account_name& contract = "eosio.token"_n ) {
		return get_currency_balance(contract, balance_symbol, act);
	}

    void create_currency( name contract, name manager, asset maxsupply ) {
        auto act =  mutable_variant_object()
            ("issuer",       manager )
            ("maximum_supply", maxsupply );

        base_tester::push_action(contract, "create"_n, contract, act );
    }

    void configureevm(const string stlos_contract, const string storage_key, const uint64_t wtlos_index) {
        action act = get_action(test_account, "setevmconfig"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
        ("user", "eosio"_n)
        ("stlos_contract", stlos_contract)
        ("storage_key", storage_key)
        ("wtlos_index", wtlos_index));
    }

    void configure(double value) {
        action act = get_action(test_account, "setratio"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
        ("user", "eosio"_n)
        ("ratio", value));
    }

    void create_core_token( symbol core_symbol = symbol{CORE_SYM} ) {
        FC_ASSERT( core_symbol.decimals() == 4, "create_core_token assumes core token has 4 digits of precision" );
        create_currency( "eosio.token"_n, config::system_account_name, asset(100000000000000, core_symbol) );
        issue(config::system_account_name, asset(10000000000000, core_symbol) );
        BOOST_REQUIRE_EQUAL( asset(10000000000000, core_symbol), get_balance( "eosio"_n, core_symbol ) );
    }

    void issue( name to, const asset& amount, name manager = config::system_account_name ) {
        base_tester::push_action( "eosio.token"_n, "issue"_n, manager, mutable_variant_object()
            ("to",      to )
            ("quantity", amount )
            ("memo", ""));
    }

    transaction_trace_ptr settf(const uint64_t amount) {
        signed_transaction trx;
        action act = get_action(test_account, "settf"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
			    ("amount", amount));
        trx.actions.emplace_back(act);
        set_transaction_headers(trx);
        trx.sign( get_private_key("eosio"_n, "active"), control->get_chain_id());
        return push_transaction(trx);
    }

    transaction_trace_ptr setecondev(const uint64_t amount) {
        signed_transaction trx;
        action act = get_action(test_account, "setecondev"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
			    ("amount", amount));
        trx.actions.emplace_back(act);
        set_transaction_headers(trx);
        trx.sign( get_private_key("eosio"_n, "active"), control->get_chain_id());
        return push_transaction(trx);
    }

    transaction_trace_ptr setevmstake(const uint64_t amount) {
        signed_transaction trx;
        action act = get_action(test_account, "setevmstake"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
			    ("amount", amount));
        trx.actions.emplace_back(act);
        set_transaction_headers(trx);
        trx.sign( get_private_key("eosio"_n, "active"), control->get_chain_id());
        return push_transaction(trx);
    }

    transaction_trace_ptr setrex(const uint64_t amount) {
        signed_transaction trx;
        action act = get_action(test_account, "setrex"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
			    ("amount", amount));
        trx.actions.emplace_back(act);
        set_transaction_headers(trx);
        trx.sign( get_private_key("eosio"_n, "active"), control->get_chain_id());
        return push_transaction(trx);
    }

    transaction_trace_ptr setdrawdown(const uint64_t amount) {
        signed_transaction trx;
        action act = get_action(test_account, "setdrawdown"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
			    ("amount", amount));
        trx.actions.emplace_back(act);
        set_transaction_headers(trx);
        trx.sign( get_private_key("eosio"_n, "active"), control->get_chain_id());
        return push_transaction(trx);
    }

    transaction_trace_ptr delpayout(const name to) {
        signed_transaction trx;
        action act = get_action(test_account, "delpayout"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo()
			    ("to", to));
        trx.actions.emplace_back(act);
        set_transaction_headers(trx);
        trx.sign( get_private_key("eosio"_n, "active"), control->get_chain_id());
        return push_transaction(trx);
    }

    transaction_trace_ptr payout() {
        signed_transaction trx;
        action act = get_action(test_account, "pay"_n, vector<permission_level>{{"eosio"_n, config::active_name}},
			mvo());
        trx.actions.emplace_back(act);
        set_transaction_headers(trx);
        trx.sign( get_private_key("eosio"_n, "active"), control->get_chain_id() );
        return push_transaction(trx);
    }

    fc::variant get_payout(name to) {
      vector<char> data = get_row_by_account( test_account, test_account, "payouts"_n, to );
      return data.empty() ? fc::variant() : tedp_abi_ser.binary_to_variant("payout", data, abi_serializer_max_time);
    }

    uint32_t now() {
	   return (control->pending_block_time().time_since_epoch().count() / 1000000);
    }

    template<typename Lambda>
    void validate_payout(Lambda&& func, name payout_name, uint64_t amount, uint64_t interval, bool is_new = true) {
        fc::variant payout;
        uint64_t previous_last_payout = 0;
        if(!is_new) {
            payout = get_payout(payout_name);
            BOOST_REQUIRE(!payout.is_null());
            previous_last_payout = payout["last_payout"].as<uint64_t>();
        }

        auto trace = func(amount);
        BOOST_REQUIRE_EQUAL(transaction_receipt::executed, trace->receipt->status);
        payout = get_payout(payout_name);

        // cout << payout_name << ": " << payout << endl;

        BOOST_REQUIRE(!payout.is_null());
        BOOST_REQUIRE_EQUAL(payout["to"].as<name>(), payout_name);
        BOOST_REQUIRE_EQUAL(payout["interval"].as<uint64_t>(), interval);
        BOOST_REQUIRE_EQUAL(payout["amount"].as<uint64_t>(), amount);
        if(is_new) {
            BOOST_REQUIRE_EQUAL(payout["last_payout"].as<uint64_t>(), now());
        } else {
            BOOST_REQUIRE_EQUAL(payout["last_payout"].as<uint64_t>(), previous_last_payout);
        }
    };

    template<typename Lambda>
    void validate_payout_del(Lambda&& func, name payout_name) {
        fc::variant payout = get_payout(payout_name);
        BOOST_REQUIRE(!payout.is_null());
        auto trace = func(payout_name);
        BOOST_REQUIRE_EQUAL(transaction_receipt::executed, trace->receipt->status);

        payout = get_payout(payout_name);
        BOOST_REQUIRE(payout.is_null());
    };
};
