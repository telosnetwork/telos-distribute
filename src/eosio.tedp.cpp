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

// Storage key for STLOS' WTLOS balance in eosio.evm AccountStates table
const std::string stlos_balance_storage_key = "0x1f26a95fb88c50fec75762c1fa2f66d147ee08e2adad92bb20a5d9e530c53abb";

// stlos address on Telos EVM
const std::string stlos = "0x85Ea6e3e3ee1db508236510B57c65251cF72191d";

// WTLOS Account State key
const uint64_t wtlos_key = 102;

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
    require_auth(get_self());
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
    uint64_t now_ms = current_time_point().sec_since_epoch();
    bool payouts_made = false;
    for (auto itr = payouts.begin(); itr != payouts.end(); itr++)
    {
        auto p = *itr;
        uint64_t time_since_last_payout = now_ms - p.last_payout;

        uint64_t payouts_due = time_since_last_payout / p.interval;

        if (payouts_due == 0)
            continue;

        payouts_made = true;
        payouts.modify(itr, get_self(), [&](auto &p) {
            p.last_payout = now_ms;
        });
        uint64_t total_due = (payouts_due * p.amount) * 10000;

        if (p.to == REX_CONTRACT)
        {
            double rex_fixed_ratio = (configuration.get().ratio  * 1.0) / 100;
            double evm_balance_ratio = getbalanceratio();
            uint64_t payout_amount = 0;
            asset payout;

            if(evm_balance_ratio >= 0){
                payout_amount = round((total_due / (1 + evm_balance_ratio)));
                if(evm_balance_ratio != 0 && rex_fixed_ratio != 1){
                    auto payout_left = total_due - payout_amount;
                    payout_amount = total_due - (payout_left * rex_fixed_ratio);
                }
                payout = asset(payout_amount, CORE_SYM);
                action(permission_level{_self, name("active")}, SYSTEM_ACCOUNT, name("distviarex"), make_tuple(get_self(), payout)).send();
            }

            if(evm_balance_ratio != 0 && payout_amount < total_due){
                payout = asset((total_due - payout_amount), CORE_SYM);
                action(permission_level{_self, name("active")}, CORE_SYM_ACCOUNT, name("transfer"), make_tuple(get_self(), EVM_CONTRACT, payout, stlos )).send();
            }
        }
        else
        {
            // transfer
            asset total_payout = asset(total_due , CORE_SYM);
            action(permission_level{_self, name("active")}, CORE_SYM_ACCOUNT, name("transfer"), make_tuple(get_self(), p.to, total_payout, std::string("TEDP Funding"))).send();
        }
    }
    check(payouts_made, "No payouts are due");
}

ACTION tedp::setconfig(uint64_t ratio_value) {
   require_auth(get_self());
   check(ratio_value < 200 && ratio_value > 0, "Ratio value must be between 0 and 200%");
   auto entry_stored = configuration.get_or_create(get_self(), config_row);
   entry_stored.ratio = ratio_value;
   configuration.set(entry_stored, get_self());
}

double tedp::getbalanceratio()
{
    eosio_evm::account_state_table account_states(EVM_CONTRACT, wtlos_key);
    eosio_evm::account_table accounts(EVM_CONTRACT, EVM_CONTRACT.value);
    rex_pool_table rex_pool(SYSTEM_ACCOUNT, SYSTEM_ACCOUNT.value);

    uint256_t evm_balance;

    auto account_states_by_key = account_states.get_index<"bykey"_n>();
    auto account_state  = account_states_by_key.find(eosio_evm::toChecksum256(intx::from_string<uint256_t>(stlos_balance_storage_key)));
    if(account_state != account_states_by_key.end()){
        evm_balance = account_state->value;
    }
    auto accounts_by_address = accounts.get_index<"byaddress"_n>();
    auto account = accounts_by_address.find(eosio_evm::toChecksum256(intx::from_string<uint256_t>(stlos)));
    if(account != accounts_by_address.end()){
        evm_balance = evm_balance + account->balance;
    }

    if(evm_balance == 0){
        return 0.0;
    }
    const auto evm_total = atoi(intx::to_string(intx::from_string<uint256_t>(intx::to_string(evm_balance, 10)) / pow(10, 14)).c_str()); // Loosing precision but ratio won't be affected much
    const auto rex_total = (rex_pool.begin() != rex_pool.end()) ? rex_pool.begin()->total_lendable.amount : 0;
    return (rex_total == 0) ? -1.0 : evm_total / double(rex_total);
}

