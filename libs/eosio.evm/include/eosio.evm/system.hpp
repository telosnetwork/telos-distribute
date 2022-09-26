#pragma once

#include <cmath>
#include <eosio/privileged.hpp>

namespace eosio_evm
{
    // Ramcore symbol for price lookup
    //static constexpr eosio::symbol_code RAMCORE_SYMBOL_CODE = eosio::symbol_code("RAMCORE");
    static constexpr eosio::symbol RAMCORE_SYMBOL = eosio::symbol(eosio::symbol_code("RAMCORE"), 4);

    // borrowed from system contracts
    struct [[eosio::table]] eosio_global_state : eosio::blockchain_parameters 
    {
        uint64_t free_ram() const { return max_ram_size - total_ram_bytes_reserved; }

        uint64_t max_ram_size = 12ll * 1024 * 1024 * 1024;
        uint64_t total_ram_bytes_reserved = 0;
        int64_t total_ram_stake = 0;

        eosio::block_timestamp last_producer_schedule_update;
        eosio::block_timestamp last_proposed_schedule_update;
        eosio::time_point last_pervote_bucket_fill;
        int64_t pervote_bucket = 0;
        int64_t perblock_bucket = 0;
        uint32_t total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
        int64_t total_activated_stake = 0;
        eosio::time_point thresh_activated_stake_time;
        uint16_t last_producer_schedule_size = 0;
        double total_producer_vote_weight = 0; /// the sum of all producer votes
        eosio::block_timestamp last_name_close;
        uint32_t block_num = 12;
        uint32_t last_claimrewards = 0;
        uint32_t next_payment = 0;
        uint16_t new_ram_per_block = 0;
        eosio::block_timestamp last_ram_increase;
        eosio::block_timestamp last_block_num; /* deprecated */
        double total_producer_votepay_share = 0;
        uint8_t revision = 0; ///< used to track version updates in the future.

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE_DERIVED( eosio_global_state, eosio::blockchain_parameters,
            (max_ram_size)(total_ram_bytes_reserved)(total_ram_stake)
            (last_producer_schedule_update)(last_proposed_schedule_update)(last_pervote_bucket_fill)
            (pervote_bucket)(perblock_bucket)(total_unpaid_blocks)(total_activated_stake)(thresh_activated_stake_time)
            (last_producer_schedule_size)(total_producer_vote_weight)(last_name_close)(block_num)(last_claimrewards)(next_payment)
            (new_ram_per_block)(last_ram_increase)(last_block_num)(total_producer_votepay_share)(revision) )
    };

    typedef eosio::singleton<"global"_n, eosio_global_state> global_state_singleton;

    struct [[eosio::table, eosio::contract("eosio.system")]] exchange_state
    {
        eosio::asset supply;

        struct connector
        {
            eosio::asset balance;
            double weight = .5;

            EOSLIB_SERIALIZE(connector, (balance)(weight))
        };

        connector base;
        connector quote;

        uint64_t primary_key() const { return supply.symbol.raw(); }

        /*
        eosio::asset convert_to_exchange(connector & reserve, const eosio::asset &payment)
        {
            const double S0 = supply.amount;
            const double R0 = reserve.balance.amount;
            const double dR = payment.amount;
            const double F = reserve.weight;

            double dS = S0 * (std::pow(1. + dR / R0, F) - 1.);
            if (dS < 0)
                dS = 0; // rounding errors
            reserve.balance += payment;
            supply.amount += int64_t(dS);
            return eosio::asset(int64_t(dS), supply.symbol);
        }

        eosio::asset convert_from_exchange(connector & reserve, const eosio::asset &tokens)
        {
            const double R0 = reserve.balance.amount;
            const double S0 = supply.amount;
            const double dS = -tokens.amount; // dS < 0, tokens are subtracted from supply
            const double Fi = double(1) / reserve.weight;

            double dR = R0 * (std::pow(1. + dS / S0, Fi) - 1.); // dR < 0 since dS < 0
            if (dR > 0)
                dR = 0; // rounding errors
            reserve.balance.amount -= int64_t(-dR);
            supply -= tokens;
            return eosio::asset(int64_t(-dR), reserve.balance.symbol);
        }

        eosio::asset convert(const eosio::asset &from, const eosio::symbol &to)
        {
            const auto &sell_symbol = from.symbol;
            const auto &base_symbol = base.balance.symbol;
            const auto &quote_symbol = quote.balance.symbol;
            eosio::check(sell_symbol != to, "cannot convert to the same symbol");

            eosio::asset out(0, to);
            if (sell_symbol == base_symbol && to == quote_symbol)
            {
                const eosio::asset tmp = convert_to_exchange(base, from);
                out = convert_from_exchange(quote, tmp);
            }
            else if (sell_symbol == quote_symbol && to == base_symbol)
            {
                const eosio::asset tmp = convert_to_exchange(quote, from);
                out = convert_from_exchange(base, tmp);
            }
            else
            {
                eosio::check(false, "invalid conversion");
            }
            return out;
        }
        eosio::asset direct_convert(const eosio::asset &from, const eosio::symbol &to)
        {
            const auto &sell_symbol = from.symbol;
            const auto &base_symbol = base.balance.symbol;
            const auto &quote_symbol = quote.balance.symbol;
            eosio::check(sell_symbol != to, "cannot convert to the same symbol");

            eosio::asset out(0, to);
            if (sell_symbol == base_symbol && to == quote_symbol)
            {
                out.amount = get_bancor_output(base.balance.amount, quote.balance.amount, from.amount);
                base.balance += from;
                quote.balance -= out;
            }
            else if (sell_symbol == quote_symbol && to == base_symbol)
            {
                out.amount = get_bancor_output(quote.balance.amount, base.balance.amount, from.amount);
                quote.balance += from;
                base.balance -= out;
            }
            else
            {
                eosio::check(false, "invalid conversion");
            }
            return out;
        }
        */
        /**
       * Given two connector balances (inp_reserve and out_reserve), and an incoming amount
       * of inp, this function calculates the delta out using Banacor equation.
       *
       * @param inp - input amount, same units as inp_reserve
       * @param inp_reserve - the input connector balance
       * @param out_reserve - the output connector balance
       *
       * @return int64_t - conversion output amount
       */
      /*
        static int64_t get_bancor_output(int64_t inp_reserve,
                                         int64_t out_reserve,
                                         int64_t inp)
        {
            const double ib = inp_reserve;
            const double ob = out_reserve;
            const double in = inp;

            int64_t out = int64_t((in * ob) / (ib + in));

            if (out < 0)
                out = 0;

            return out;
        }
        */
        static int64_t get_bancor_input(int64_t out_reserve,
                                        int64_t inp_reserve,
                                        int64_t out)
        {
            const double ob = out_reserve;
            const double ib = inp_reserve;

            int64_t inp = (ib * out) / (ob - out);

            if (inp < 0)
                inp = 0;

            return inp;
        }

        EOSLIB_SERIALIZE(exchange_state, (supply)(base)(quote))
    };

    typedef eosio::multi_index<"rammarket"_n, exchange_state> rammarket;

}