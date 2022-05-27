#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

#include <eosio.evm/eosio.evm.hpp>

#include <eosio.token/eosio.token.hpp>


using namespace std;
using namespace eosio;

class [[eosio::contract("eosio.tedp")]] tedp : public contract
{
public:
    using contract::contract;
    tedp(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds), payouts(receiver, receiver.value),  configuration(receiver, receiver.value) {}

    [[eosio::action]]
    void setconfig(uint64_t ratio);

    [[eosio::action]]
    void settf(uint64_t amount);

    [[eosio::action]]
    void setecondev(uint64_t amount);

    [[eosio::action]]
    void setcoredev(uint64_t amount);

    [[eosio::action]]
    void setrex(uint64_t amount);

    [[eosio::action]]
    void delpayout(name to);

    [[eosio::action]]
    void pay();

private:
    static constexpr name CORE_SYM_ACCOUNT = name("eosio.token");
    static constexpr symbol CORE_SYM = symbol("TLOS", 4);
    static constexpr name EVM_CONTRACT = name("eosio.evm");
    static constexpr name REX_CONTRACT = name("eosio.rex");
    void setpayout(name to, uint64_t amount, uint64_t interval);
    double getbalanceratio();

    TABLE config {
        uint64_t ratio;
        EOSLIB_SERIALIZE(config, (ratio))
    } config_row;

   struct [[eosio::table,eosio::contract("eosio.system")]] rex_pool {
      uint8_t    version = 0;
      asset      total_lent;
      asset      total_unlent;
      asset      total_rent;
      asset      total_lendable;
      asset      total_rex;
      asset      namebid_proceeds;
      uint64_t   loan_num = 0;

      uint64_t primary_key()const { return 0; }
   };

    TABLE payout {
        name to;
        uint64_t amount;
        uint64_t interval;
        uint64_t last_payout;
        uint64_t primary_key() const { return to.value; }
    };

    typedef multi_index<name("payouts"), payout> payout_table;

    typedef eosio::multi_index< "rexpool"_n, rex_pool > rex_pool_table;

    typedef eosio::singleton<"config"_n, config> config_table;

    using setpayout_action = action_wrapper<name("setpayout"), &tedp::setpayout>;
    using delpayout_action = action_wrapper<name("delpayout"), &tedp::delpayout>;
    using pay_action = action_wrapper<name("payout"), &tedp::pay>;
    using setconfig_action = action_wrapper<"setconfig"_n, &tedp::setconfig>;
    payout_table payouts;
    config_table configuration;
};

