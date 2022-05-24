#include <eosio.tedp/eosio.tedp.hpp>
// tf 1mil
// econdevfunds 500k
// rex 1mil
// wps 1mil
// bpay 1760000
// total 5260000 * 12 momths / 365 days = 172931.5068 per day (63.12mil per year)
// rounding up to ensure contract always can have a surplus beyond the total max of all payouts
// const uint64_t max_drawdown_amount = 173000;

// 60sec * 60min * 24hrs
const uint64_t daily_interval = 86400;

// 60sec * 30min
const uint64_t rex_interval = 1800;

// 500k * 12 months / 365 days = 16438.3561
const uint64_t max_econdev_amount = 16438;

// 1mil * 12 months / 365 days = 32876.7123
const uint64_t max_tf_amount = 32876;

// 300k * 12 months / 365 days = 9863.0136
const uint64_t max_coredev_amount = 9863;

// 1.7mil * 12 months / 365 days / 24hrs / 2 (every half hour) = 1164.3835
const uint64_t max_rex_amount = 1165;

// Storage key in eosio.evm AccountStates table
const std::string sTLOS_WTLOS_balance_storage_key = "c526daaebcecaca2768e23b00b7f63b8c7fbc592f8bffd713e945c96cc6a6686";

// sTLOS address on Telos EVM
const std::string sTLOS = "0x222222222222222222222222222";

void tedp::settf(uint64_t amount)
{
    check(amount <= max_tf_amount, "Max amount for tf account is 32876 per day");
    setpayout(name("tf"), amount, daily_interval);
}

void tedp::setecondev(uint64_t amount)
{
    check(amount <= max_econdev_amount, "Max amount for econdevfunds account is 16438 per day");
    setpayout(name("econdevfunds"), amount, daily_interval);
}

void tedp::setcoredev(uint64_t amount)
{
    check(amount <= max_coredev_amount, "Max amount for econdevfunds account is 9863 per day");
    setpayout(name("treasury.tcd"), amount, daily_interval);
}

void tedp::setrex(uint64_t amount) {
    check(amount <= max_rex_amount, "Max amount for eosio.rex account is 1165 * 0.4 per 30min");
    setpayout(REX_CONTRACT, amount, rex_interval);
}

void tedp::setevmstake(uint64_t amount) {
    check(amount <= max_rex_amount, "Max amount for eosio.evm stlos contract is 1165 * 0.6 per 30min");
    setpayout(EVM_CONTRACT, amount, rex_interval);
}

void tedp::setpayout(name to, uint64_t amount, uint64_t interval)
{
    require_auth(name("eosio"));
    check(is_account(to), "The payee is not a valid account");
    auto itr = payouts.find(to.value);
    if (itr == payouts.end())
    {
        payouts.emplace(get_self(), [&](auto &p) {
            p.to = to;
            p.amount = amount;
            p.interval = interval;
            p.last_payout = current_time_point().sec_since_epoch();
        });
    }
    else
    {
        payouts.modify(itr, get_self(), [&](auto &p) {
            p.amount = amount;
            p.interval = interval;
        });
    }
}

ACTION tedp::delpayout(name to)
{
    require_auth(name("eosio"));
    auto itr = payouts.find(to.value);
    check(itr != payouts.end(), "Payout does not exist, can't delete");
    payouts.erase(itr);
}

ACTION tedp::pay()
{
    //uint64_t now_ms = eosio::current_time_point().sec_since_epoch();
    uint64_t now_ms = current_time_point().sec_since_epoch();
    bool payouts_made = false;
    double rex_balance_ratio = getbalanceratio();
    auto config = configuration.get();
    double rex_fixed_ratio = config.ratio;

    for (auto itr = payouts.begin(); itr != payouts.end(); itr++)
    {
        auto p = *itr;
        uint64_t time_since_last_payout = now_ms - p.last_payout;
        eosio::print("now_ms:", now_ms, " - p.last_payout:", p.last_payout, " = time_since_last_payout:", time_since_last_payout, "\n");

        uint64_t payouts_due = time_since_last_payout / p.interval;
        eosio::print("time_since_last_payout:", time_since_last_payout, " / p.interval:", p.interval, " = payments_due:", payouts_due, "\n");

        if (payouts_due == 0)
            continue;

        payouts_made = true;
        payouts.modify(itr, get_self(), [&](auto &p) {
            p.last_payout = now_ms;
        });
        uint64_t total_due = payouts_due * p.amount;
        eosio::print("payout ", payouts_due, " payouts of ", p.amount, " TLOS to: ", p.to, " ", p.amount, " TLOS with time: ", now_ms, "\n");

        if (p.to == REX_CONTRACT)
        {
            // channel_to_rex
            eosio::print("Channeling to rex\n");
            total_due = round(total_due * (1 - rex_balance_ratio));
            // TODO: APPLY FIXED RATIO
            asset rex_payout = asset(total_due, CORE_SYM);
            action(permission_level{_self, name("active")}, name("eosio"), name("distviarex"), make_tuple(get_self(), rex_payout)).send();
        }
        else
        {
            if(p.to == EVM_CONTRACT){
                eosio::print("Bridging to EVM sTLOS contract \n");
                total_due = round(total_due * rex_balance_ratio);
                // TODO: APPLY FIXED RATIO
                asset stlos_payout = asset(total_due, CORE_SYM);
                action(permission_level{get_self(), name("active")}, CORE_SYM_ACCOUNT, name("transfer"), make_tuple(get_self(), EVM_CONTRACT, stlos_payout, sTLOS )).send();
            } else {
                // transfer
                eosio::print("Transferring\n");
                asset total_payout = asset(total_due * 10000, CORE_SYM);
                action(permission_level{_self, name("active")}, CORE_SYM_ACCOUNT, name("transfer"), make_tuple(get_self(), p.to, total_payout, std::string("TEDP Funding"))).send();
            }
        }
    }

    check(payouts_made, "No payouts are due");
}

double tedp::getbalanceratio()
{
    eosio_evm::account_state_table account_states(EVM_CONTRACT, 24);
    auto account_states_by_key = account_states.get_index<"bykey"_n>();
    auto account_state  = account_states_by_key.require_find(eosio_evm::toChecksum256(intx::from_string<uint256_t>(sTLOS_WTLOS_balance_storage_key)));
    check(account_state != account_states_by_key.end(), "storage key does not exist");
    const bigint::checksum256 stlos_wtlos_balance = account_state->value;

    const auto rex_tlos_balance = eosio::token::get_balance(CORE_SYM_ACCOUNT, "eosio.rex"_n, symbol_code("TLOS") );

    double ratio = ((int)stlos_wtlos_balance / (int)rex_tlos_balance.amount);
    return ratio;
}

