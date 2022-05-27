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
const auto stlos_balance_storage_key = "c526daaebcecaca2768e23b00b7f63b8c7fbc592f8bffd713e945c96cc6a6686";

// stlos address on Telos EVM
const std::string stlos = "0xe7209d65c5BB05Ddf799b20fF0EC09E691FC3f11";

const uint64_t wtlos_key = 24;

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
    check(amount <= max_rex_amount, "Max amount for eosio.rex account is 1165 per 30min");
    setpayout(REX_CONTRACT, amount, rex_interval);
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
    bigint::checksum256 val = 'c526daaebcecaca2768e23b00b7f63b8c7fbc592f8bffd713e945c96cc6a6686';
        print("lol", intx::hex(val));
    uint64_t now_ms = current_time_point().sec_since_epoch();
    bool payouts_made = false;
    double evm_balance_ratio = getbalanceratio();
    double rex_fixed_ratio = (configuration.get().ratio  * 1.0) / 100;

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
        eosio::print(payouts_due, " payouts to: ", p.to, " with time: ", now_ms, "\n");

        if (p.to == REX_CONTRACT)
        {
            uint64_t payout_amount = 0;
            asset payout;

            if(evm_balance_ratio >= 0){
                // channel to rex if there is a rex balance
                eosio::print("Channeling to REX\n");
                payout_amount = (evm_balance_ratio > 0 && rex_fixed_ratio > 0) ? round((total_due / (1 + evm_balance_ratio)) * (1 - (rex_fixed_ratio - 1))) : total_due;
                eosio::print("Payout of ", payout_amount, " TLOS to: ", p.to, " ", payout_amount, " TLOS with time: ", now_ms, "\n");
                payout = asset(payout_amount, CORE_SYM);
                action(permission_level{_self, name("active")}, name("eosio"), name("distviarex"), make_tuple(get_self(), payout)).send();
            }

            if(evm_balance_ratio != 0 && payout_amount < total_due){
                // bridge to EVM if there is an evm balance
                eosio::print("Bridging to EVM \n");
                payout_amount = total_due - payout_amount;
                eosio::print("Payout of ", payout_amount, " TLOS to: stlos EVM", " ", payout_amount, " TLOS with time: ", now_ms, "\n");
                payout = asset(payout_amount, CORE_SYM);
                action(permission_level{_self, name("active")}, CORE_SYM_ACCOUNT, name("transfer"), make_tuple(get_self(), EVM_CONTRACT, payout, stlos )).send();
            }
        }
        else
        {
            // transfer
            eosio::print("Transferring\n");
            asset total_payout = asset(total_due * 10000, CORE_SYM);
            eosio::print("Payout of ", total_payout, " TLOS to: ", p.to, " ", total_payout, " TLOS with time: ", now_ms, "\n");
            action(permission_level{_self, name("active")}, CORE_SYM_ACCOUNT, name("transfer"), make_tuple(get_self(), p.to, total_payout, std::string("TEDP Funding"))).send();
        }
    }

    check(payouts_made, "No payouts are due");
}

ACTION tedp::setconfig(uint64_t ratio_value) {
   require_auth("eosio"_n);
   check(ratio_value < 200 && ratio_value > 0, "Ratio value must be between 0 and 200%");
   auto entry_stored = configuration.get_or_create("eosio"_n, config_row);
   entry_stored.ratio = ratio_value;
   configuration.set(entry_stored, "eosio"_n);
}

double tedp::getbalanceratio()
{
    eosio_evm::account_state_table account_states(EVM_CONTRACT, wtlos_key);
    rex_pool_table rex_pool(get_self(), get_self().value);

    eosio::print("Looking for storage key: ", eosio::checksum256(eosio_evm::toBin(stlos_balance_storage_key)), "\n");

    auto account_states_by_key = account_states.get_index<"bykey"_n>();

    auto account_state  = account_states_by_key.find(eosio::checksum256(eosio_evm::toBin(stlos_balance_storage_key)));

    if(account_state == account_states_by_key.end() || (int) account_state->value == 0){
        eosio::print("Key not found");
        return 0.0;
    }

    eosio::print("Key found with value: ", (int) account_state->value, "\n");

    const bigint::checksum256 stlos_balance = account_state->value;

    const auto rex_pool_row = rex_pool.find(0);

    const auto rex_balance = (rex_pool_row != rex_pool.end()) ? rex_pool_row->total_lendable.amount : 0;

    return (rex_balance == 0) ? -1.0 : double ((int) stlos_balance / ((int) rex_balance) * 1.0);
}

