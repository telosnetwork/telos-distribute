#include <eosio.tedp/eosio.tedp.hpp>

#define ERR_MSG(acc, amount, interval) \
    "Max amount for " + acc.to_string() + " account is " + to_string(amount) + " per " + interval


void tedp::settf(uint64_t amount) {
    check(
        amount <= max_tf_amount,
        ERR_MSG(TF_ACCOUNT, max_tf_amount, "day"));

    setpayout(TF_ACCOUNT, amount, daily_interval);
}

void tedp::setecondev(uint64_t amount) {
    check(
        amount <= max_econdev_amount,
        ERR_MSG(ECONDEV_ACCOUNT, max_econdev_amount, "day"));

    setpayout(ECONDEV_ACCOUNT, amount, daily_interval);
}

void tedp::setcoredev(uint64_t amount) {
    check(
        amount <= max_coredev_amount,
        ERR_MSG(COREDEV_ACCOUNT, max_coredev_amount, "day"));

    setpayout(COREDEV_ACCOUNT, amount, daily_interval);
}

void tedp::setrex(uint64_t amount) {
    check(
        amount <= max_rex_amount,
        ERR_MSG(REX_ACCOUNT, max_rex_amount, "30min"));

    setpayout(REX_ACCOUNT, amount, rex_interval);
}

void tedp::setignite(uint64_t amount) {
    check(
        amount <= max_ignitegrants_amount,
        ERR_MSG(IGNITE_ACCOUNT, max_ignitegrants_amount, "day"));

    setpayout(IGNITE_ACCOUNT, amount, daily_interval);
}

void tedp::setfuel(uint64_t amount) {
    check(
        amount <= max_tlosfuel_amount,
        ERR_MSG(FUEL_ACCOUNT, max_tlosfuel_amount, "day"));

    setpayout(FUEL_ACCOUNT, amount, daily_interval);
}

void tedp::setpayout(name to, uint64_t amount, uint64_t interval)
{
    require_auth(SYSTEM_ACCOUNT);
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

void tedp::delpayout(name to)
{
    require_auth(SYSTEM_ACCOUNT);
    auto itr = payouts.find(to.value);
    check(itr != payouts.end(), "Payout does not exist, can't delete");
    payouts.erase(itr);
}

void tedp::pay()
{
    uint64_t now_ms = current_time_point().sec_since_epoch();
    bool payouts_made = false;
    double evm_balance_ratio = getbalanceratio();
    string stlos_contract = configuration.get().stlos_contract;

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

        if (p.to == REX_ACCOUNT)
        {
            uint64_t payout_amount = 0;
            asset payout;

            if(evm_balance_ratio >= 0){
                payout_amount = round((total_due / (1 + evm_balance_ratio)));
                payout = asset(payout_amount, CORE_SYM);
                action(
                    permission_level{_self, "active"_n},
                    SYSTEM_ACCOUNT, "distviarex"_n,
                    make_tuple(get_self(), payout)
                ).send();
            }

            if(evm_balance_ratio != 0 && payout_amount < total_due){
                payout = asset((total_due - payout_amount), CORE_SYM);
                action(
                    permission_level{_self, "active"_n},
                    CORE_SYM_ACCOUNT,
                    "transfer"_n,
                    make_tuple(get_self(), EVM_ACCOUNT, payout, stlos_contract)
                ).send();
            }
        }
        else
        {
            // transfer
            asset total_payout = asset(total_due , CORE_SYM);
            action(
                permission_level{_self, "active"_n},
                CORE_SYM_ACCOUNT,
                "transfer"_n,
                make_tuple(get_self(), p.to, total_payout, std::string("TEDP Funding"))
            ).send();
        }
    }
    check(payouts_made, "No payouts are due");
}

void tedp::setratio(uint64_t ratio_value) {
   require_auth(SYSTEM_ACCOUNT);
   auto entry_stored = configuration.get_or_create(get_self(), config_row);
   check(ratio_value < 200 && ratio_value > 0, "Ratio value must be between 0 and 200%");
   entry_stored.ratio = ratio_value;
   configuration.set(entry_stored, get_self());
}

void tedp::setevmconfig(string stlos_contract, eosio::checksum256 storage_key, uint64_t wtlos_index) {
   require_auth(SYSTEM_ACCOUNT);
   auto entry_stored = configuration.get_or_create(get_self(), config_row);
   entry_stored.stlos_contract = stlos_contract;
   entry_stored.storage_key = storage_key;
   entry_stored.wtlos_index = wtlos_index;
   configuration.set(entry_stored, get_self());
}

double tedp::getbalanceratio()
{
    auto conf = configuration.get();
    eosio_evm::account_state_table account_states(EVM_ACCOUNT, conf.wtlos_index);
    eosio_evm::account_table accounts(EVM_ACCOUNT, EVM_ACCOUNT.value);
    rex_pool_table rex_pool(SYSTEM_ACCOUNT, SYSTEM_ACCOUNT.value);

    uint256_t evm_balance = 0;
    double fixed_ratio = (conf.ratio  * 1.0) / 100;

    auto account_states_by_key = account_states.get_index<"bykey"_n>();
    auto account_state  = account_states_by_key.find(conf.storage_key);
    if(account_state != account_states_by_key.end()){
        evm_balance = account_state->value;
    }
    auto accounts_by_address = accounts.get_index<"byaddress"_n>();
    auto account = accounts_by_address.find(eosio::checksum256(eosio_evm::toBin(conf.stlos_contract.erase(0,2))));
    if(account != accounts_by_address.end()){
        evm_balance = evm_balance + account->balance;
    }

    if(evm_balance == 0){
        return 0.0;
    }

    const auto evm_total = atoi(intx::to_string(intx::from_string<uint256_t>(intx::to_string(evm_balance, 10)) / pow(10, 14)).c_str()); // Loosing precision but ratio won't be affected much
    const auto rex_total = (rex_pool.begin() != rex_pool.end()) ? rex_pool.begin()->total_lendable.amount : 0;
    return (rex_total == 0) ? -1.0 : (evm_total * fixed_ratio) / double(rex_total);
}
