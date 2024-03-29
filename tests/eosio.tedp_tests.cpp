#include <boost/test/unit_test.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/wast_to_wasm.hpp>

// #include <cstdlib>
#include <iostream>
#include <sstream>
#include <fc/log/logger.hpp>
#include <eosio/chain/exceptions.hpp>
#include <Runtime/Runtime.h>

#include "eosio.tedp_tester.hpp"

using namespace std;


BOOST_AUTO_TEST_SUITE(eosio_tedp_tests)

BOOST_FIXTURE_TEST_CASE( set_payouts, eosio_tedp_tester ) try {

    // set each payout to max value
    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return settf(amount); },
        TF_ACCOUNT, max_tf_amount, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setrex(amount); },
        REX_ACCOUNT, max_rex_amount, rex_interval);

    // validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setecondev(amount); },
    //    ECONDEV_ACCOUNT, max_econdev_amount, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setcoredev(amount); },
        COREDEV_ACCOUNT, max_coredev_amount, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setignite(amount); },
        IGNITE_ACCOUNT, max_ignitegrants_amount, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setfuel(amount); },
        FUEL_ACCOUNT, max_tlosfuel_amount, daily_interval);

    // set to max value - 10
    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return settf(amount); },
        TF_ACCOUNT, max_tf_amount - 10, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setrex(amount); },
        REX_ACCOUNT, max_rex_amount - 10, rex_interval);

    // validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setecondev(amount); },
    //    ECONDEV_ACCOUNT, max_econdev_amount - 10, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setcoredev(amount); },
        COREDEV_ACCOUNT, max_coredev_amount - 10, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setignite(amount); },
        IGNITE_ACCOUNT, max_ignitegrants_amount - 10, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setfuel(amount); },
        FUEL_ACCOUNT, max_tlosfuel_amount - 10, daily_interval);

    // assert we cant set values over max
    BOOST_REQUIRE_EXCEPTION(
        settf(max_tf_amount + 1),
		eosio_assert_message_exception,
        eosio_assert_message_is(
            "Max amount for " + TF_ACCOUNT.to_string() + " account is " + std::to_string(max_tf_amount) + " per day"));

    BOOST_REQUIRE_EXCEPTION(
        setrex(max_rex_amount + 1),
		eosio_assert_message_exception,
        eosio_assert_message_is(
            "Max amount for " + REX_ACCOUNT.to_string() + " account is " + std::to_string(max_rex_amount) + " per 30min"));

    // BOOST_REQUIRE_EXCEPTION(
    //     setecondev(max_econdev_amount + 1),
	// 	eosio_assert_message_exception,
    //     eosio_assert_message_is(
    //         "Max amount for " + ECONDEV_ACCOUNT.to_string() + " account is " + std::to_string(max_econdev_amount) + " per day"));

    BOOST_REQUIRE_EXCEPTION(
        setcoredev(max_coredev_amount + 1),
		eosio_assert_message_exception,
        eosio_assert_message_is(
            "Max amount for " + COREDEV_ACCOUNT.to_string() + " account is " + std::to_string(max_coredev_amount) + " per day"));

    BOOST_REQUIRE_EXCEPTION(
        setignite(max_ignitegrants_amount + 1),
		eosio_assert_message_exception,
        eosio_assert_message_is(
            "Max amount for " + IGNITE_ACCOUNT.to_string() + " account is " + std::to_string(max_ignitegrants_amount) + " per day"));

    BOOST_REQUIRE_EXCEPTION(
        setfuel(max_tlosfuel_amount + 1),
		eosio_assert_message_exception,
        eosio_assert_message_is(
            "Max amount for " + FUEL_ACCOUNT.to_string() + " account is " + std::to_string(max_tlosfuel_amount) + " per day"));

    // validate payout deletions
    for(name n : payees) {
        cout << "doing payout for " << n.to_string() << "...";
        validate_payout_del([&](const name payout_name) -> transaction_trace_ptr {
            return delpayout(payout_name);
        }, n);
        cout << " done." << endl;
    }

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( pay_flow, eosio_tedp_tester ) try {
    transfer( config::system_account_name, test_account, core_sym::from_string("139873804.3119"), config::system_account_name );
    const asset init_balance = core_sym::from_string("30000.0000");
    const std::vector<account_name> accounts = { "aliceaccount"_n, "bobbyaccount"_n, "carolaccount"_n, "emilyaccount"_n };
    account_name alice = accounts[0], bob = accounts[1], carol = accounts[2], emily = accounts[3];
    setup_rex_accounts( accounts, init_balance );

    const int64_t init_stake = get_voter_info( alice )["staked"].as<int64_t>();

    const asset payment = core_sym::from_string("25000.0000");
    BOOST_REQUIRE_EQUAL( success(),                              buyrex( alice, payment ) );
    BOOST_REQUIRE_EQUAL( payment,                                get_rex_vote_stake(alice) );
    BOOST_REQUIRE_EQUAL( get_rex_vote_stake(alice).get_amount(), get_voter_info(alice)["staked"].as<int64_t>() - init_stake );

    const asset fee = core_sym::from_string("50.0000");

    configure(40);
    configureevm("0x85Ea6e3e3ee1db508236510B57c65251cF72191d", "1f26a95fb88c50fec75762c1fa2f66d147ee08e2adad92bb20a5d9e530c53abb", 102);

    BOOST_REQUIRE_EQUAL( success(),                              rentcpu( emily, bob, fee ) );

    BOOST_REQUIRE_EQUAL( success(),                              updaterex( alice ) );
    BOOST_REQUIRE_EQUAL( payment + fee,                          get_rex_vote_stake(alice) );
    BOOST_REQUIRE_EQUAL( get_rex_vote_stake(alice).get_amount(), get_voter_info( alice )["staked"].as<int64_t>() - init_stake );

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return settf(amount); }, 
        name("tf"), max_tf_amount, daily_interval);

    // validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setecondev(amount); }, 
    //     name("econdevfunds"), max_econdev_amount, daily_interval);

    validate_payout([&](uint64_t amount) -> transaction_trace_ptr { return setrex(amount); }, 
        name("eosio.rex"), max_rex_amount, rex_interval);

    auto dump_trace = [&](transaction_trace_ptr trace_ptr) -> transaction_trace_ptr {
        cout << endl << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
        for(auto trace : trace_ptr->action_traces) {
            cout << "action_name trace: " << trace.act.name.to_string() << endl;
            //TODO: split by new line character, loop and indent output
            cout << trace.console << endl << endl;
        }
        cout << endl << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl << endl;
        return trace_ptr;
    };

    produce_blocks();
    produce_block(fc::seconds(daily_interval));
    produce_blocks(10);

    fc::variant initial_rex_pool = get_rex_pool();
    asset initial_total_unlent = initial_rex_pool["total_unlent"].as<asset>();
    asset initial_total_lendable = initial_rex_pool["total_lendable"].as<asset>();
    uint64_t last_payout      = get_payout("tf"_n)["last_payout"].as<uint64_t>();

    asset initial_tf_balance = get_balance("tf"_n);
    asset initial_econ_balance = get_balance("econdevfunds"_n);
    asset initial_evm_balance = get_balance("eosio.evm"_n);

    auto trace = dump_trace(payout());
    BOOST_REQUIRE_EQUAL(transaction_receipt::executed, trace->receipt->status);

    vector<name> payout_names { name("eosio.rex"), name("tf") };

    for(const auto &payout : payout_names) {
        fc::variant payout_info = get_payout(payout);

        uint64_t time_since_last_payout = now() - last_payout;
        uint64_t payouts_due = time_since_last_payout / payout_info["interval"].as<uint64_t>();
        uint64_t total_due = payouts_due * payout_info["amount"].as<uint64_t>();
        asset total_payout = asset(total_due * 10000, symbol(4, "TLOS"));

        if (payout_info["to"].as<name>() == "eosio.rex"_n) {
            fc::variant rex_pool = get_rex_pool();
            BOOST_REQUIRE_EQUAL(rex_pool["total_unlent"].as<asset>(), initial_total_unlent + total_payout);
            BOOST_REQUIRE_EQUAL(rex_pool["total_lendable"].as<asset>(), initial_total_lendable + total_payout);
        } else {
            auto initial_balance = initial_evm_balance;
            if(payout_info["to"].as<name>() != "eosio.evm"_n) {
                initial_balance = (payout == "econdevfunds"_n ? initial_econ_balance : initial_tf_balance);
                cout << "initial_balance: " << initial_balance << endl;
            }
            BOOST_REQUIRE_EQUAL(get_balance(payout), initial_balance + total_payout);
        }
    }

    produce_blocks();
    produce_block(fc::seconds(daily_interval * 2));
    produce_blocks(10);

    initial_rex_pool = get_rex_pool();
    initial_total_unlent = initial_rex_pool["total_unlent"].as<asset>();
    initial_total_lendable = initial_rex_pool["total_lendable"].as<asset>();
    last_payout      = get_payout("tf"_n)["last_payout"].as<uint64_t>();

    initial_tf_balance = get_balance("tf"_n);
    initial_econ_balance = get_balance("econdevfunds"_n);
    initial_evm_balance = get_balance("eosio.evm"_n);

    trace = dump_trace(payout());
    BOOST_REQUIRE_EQUAL(transaction_receipt::executed, trace->receipt->status);

    for(const auto &payout : payout_names) {
        fc::variant payout_info = get_payout(payout);

        uint64_t time_since_last_payout = now() - last_payout;
        uint64_t payouts_due = time_since_last_payout / payout_info["interval"].as<uint64_t>();
        uint64_t total_due = payouts_due * payout_info["amount"].as<uint64_t>();
        asset total_payout = asset(total_due * 10000, symbol(4, "TLOS"));

        if (payout_info["to"].as<name>() == "eosio.rex"_n) {
            fc::variant rex_pool = get_rex_pool();
            BOOST_REQUIRE_EQUAL(rex_pool["total_unlent"].as<asset>(), initial_total_unlent + total_payout);
            BOOST_REQUIRE_EQUAL(rex_pool["total_lendable"].as<asset>(), initial_total_lendable + total_payout);
        } else {
            auto initial_balance = initial_evm_balance;
            if(payout_info["to"].as<name>() != "eosio.evm"_n) {
                initial_balance = (payout == "econdevfunds"_n ? initial_econ_balance : initial_tf_balance);
            }
            BOOST_REQUIRE_EQUAL(get_balance(payout), initial_balance + total_payout);
        }
    }

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
