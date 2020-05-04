#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>
#include "system_kick.cpp"

namespace eosiosystem {

//    const int64_t  min_pervote_daily_pay = 100'0000;
//    const int64_t  min_activated_stake   = 150'000'000'0000;
//    const double   perblock_rate         = 0.0025;           // 0.25%
//    const double   standby_rate          = 0.0075;           // 0.75%
//    const uint32_t blocks_per_year       = 52*7*24*2*3600;   // half seconds per year
//    const uint32_t seconds_per_year      = 52*7*24*3600;
//    const uint32_t blocks_per_day        = 2 * 24 * 3600;
//    const uint32_t blocks_per_hour       = 2 * 3600;
//    const int64_t  useconds_per_day      = 24 * 3600 * int64_t(1000000);
//    const int64_t  useconds_per_year     = seconds_per_year*1000000ll;

   void system_contract::onblock( ignore<block_header> ) {
        using namespace eosio;

        require_auth(get_self());

        block_timestamp timestamp;
        name producer;
        _ds >> timestamp >> producer;

        _gstate.block_num++;
        if (_gstate.thresh_activated_stake_time == time_point()) {
            if(_gstate.block_num >= block_num_network_activation && _gstate.total_producer_vote_weight > 0) {
                _gstate.thresh_activated_stake_time = current_time_point();
                _gstate.last_claimrewards = timestamp.slot;
            }
            return;
        }
     
        if (_gstate.last_pervote_bucket_fill == time_point()) _gstate.last_pervote_bucket_fill = current_time_point();

        if(check_missed_blocks(timestamp, producer)) {
            update_missed_blocks_per_rotation();
            reset_schedule_metrics(producer);
        }

        /**
         * At startup the initial producer may not be one that is registered / elected
         * and therefore there may be no producer object for them.
         */
        auto prod = _producers.find( producer.value );
        if ( prod != _producers.end() ) {
            _gstate.total_unpaid_blocks++;
            _producers.modify( prod, same_payer, [&](auto& p ) {
                p.unpaid_blocks++;
                p.lifetime_produced_blocks++;
            });
        }

        recalculate_votes();

        /// only update block producers once every minute, block_timestamp is in half seconds
        if( timestamp.slot - _gstate.last_producer_schedule_update.slot > 120 ) {
            update_elected_producers( timestamp );

            if( (timestamp.slot - _gstate.last_name_close.slot) > blocks_per_day ) {
                name_bid_table bids(_self, _self.value);
                auto idx = bids.get_index<"highbid"_n>();
                auto highest = idx.lower_bound( std::numeric_limits<uint64_t>::max()/2 );
                if( highest != idx.end() &&
                    highest->high_bid > 0 &&
                    (current_time_point() - highest->last_bid_time) > microseconds(useconds_per_day) &&
                    _gstate.thresh_activated_stake_time > time_point() &&
                    (current_time_point() - _gstate.thresh_activated_stake_time) > microseconds(14 * useconds_per_day)
                ) {
                _gstate.last_name_close = timestamp;
                channel_namebid_to_rex( highest->high_bid );
                idx.modify( highest, same_payer, [&]( auto& b ){
                    b.high_bid = -b.high_bid;
                });
                }
            }
        }

        //called once per day to set payments snapshot
        if (_gstate.last_claimrewards + uint32_t(3600) <= timestamp.slot) { //172800 blocks in a day
            claimrewards_snapshot();
            _gstate.last_claimrewards = timestamp.slot;
        }
   }

   using namespace eosio;
   void system_contract::claimrewards( const name& owner ) {
        require_auth(owner);
        check(_gstate.thresh_activated_stake_time > time_point(), "cannot claim rewards until the chain is activated (1,000,000 blocks produced)");

        const auto& prod = _producers.get( owner.value );
        check( prod.active(), "producer does not have an active key" );

        auto p = _payments.find(owner.value);
        check(p != _payments.end(), "No payment exists for account");
        auto prod_payment = *p;
        auto pay_amount = prod_payment.pay;

        //NOTE: consider resetting producer's last claim time to 0 here, instead of during snapshot.
        {
            token::transfer_action transfer_act{ token_account, { bpay_account, active_permission } };
            transfer_act.send( bpay_account, owner, pay_amount, "Producer/Standby Payment" );
        }

        _payments.erase(p);
   }

   void system_contract::claimrewards_snapshot() {
        check(_gstate.thresh_activated_stake_time > time_point(), "cannot take snapshot until chain is activated");

        //skips action, since there are no rewards to claim
        if (_gstate.total_unpaid_blocks <= 0) { 
            return;
        }

        auto ct = current_time_point();

        const asset token_supply = eosio::token::get_supply(token_account, core_symbol().code() );
        const auto usecs_since_last_fill = (ct - _gstate.last_pervote_bucket_fill).count();
        
        if (usecs_since_last_fill > 0 && _gstate.last_pervote_bucket_fill > time_point())
        {
            double bpay_rate = double(_gpayrate.bpay_rate) / double(100000); //NOTE: both bpay_rate and divisor were int64s which evaluated to 0. The divisor must be a double to get percentage.
            auto to_workers = static_cast<int64_t>((12 * double(_gpayrate.worker_amount) * double(usecs_since_last_fill)) / double(useconds_per_year));
            auto to_producers = static_cast<int64_t>((bpay_rate * double(token_supply.amount) * double(usecs_since_last_fill)) / double(useconds_per_year));
            auto new_tokens = to_workers + to_producers;

            //NOTE: This line can cause failure if eosio.tedp doesn't have a balance emplacement
            asset tedp_balance = eosio::token::get_balance(token_account, tedp_account, core_symbol().code());
            
            int64_t transfer_tokens = 0;
            int64_t issue_tokens = 0;
            if (tedp_balance.amount > 0) {
                if (tedp_balance.amount >= new_tokens) {
                    transfer_tokens = new_tokens;
                } else {
                    transfer_tokens = tedp_balance.amount;
                    issue_tokens = new_tokens - transfer_tokens; 
                }
            } else {
                issue_tokens = new_tokens;
            }
            
            if (transfer_tokens > 0) {
                token::transfer_action transfer_act{ token_account, { tedp_account, active_permission } };
                transfer_act.send( tedp_account, get_self(), asset(transfer_tokens, core_symbol()), "TEDP: Inflation offset" );
            }

            token::transfer_action transfer_act{ token_account, { get_self(), active_permission } };  

            if (issue_tokens > 0) {
                token::issue_action issue_action{ token_account, { get_self(), active_permission }};
                issue_action.send(get_self(), asset(issue_tokens, core_symbol()), "Issue new TLOS tokens");
            }
            
            if(to_workers > 0) {
                transfer_act.send(get_self(), works_account, asset(to_workers, core_symbol()), "Transfer worker proposal share to works.decide account");
            }
            
            if(to_producers > 0) {
                transfer_act.send(get_self(), bpay_account, asset(to_producers, core_symbol()), "Transfer producer share to per-block bucket");
            }
            
            _gstate.perblock_bucket += to_producers;
            _gstate.last_pervote_bucket_fill = ct;
        }

        //sort producers table
        auto sortedprods = _producers.get_index<"prototalvote"_n>();
        
        uint32_t sharecount = 0;

        //calculate shares, should be between 2 and 72 shares
        for (const auto &prod : sortedprods)
        {
            if (prod.active()) { 			//only count activated producers
                if (sharecount <= 42) {
                    sharecount += 2; 		//top producers count as double shares
                } else if (sharecount >= 43 && sharecount < 72) {
                    sharecount++;
                } else
                    break; 					//no need to count past 72 shares
            }
        }
        
        auto shareValue = (_gstate.perblock_bucket / sharecount);
        int32_t index = 0;

        for (const auto &prod : sortedprods) {

            if (!prod.active()) //skip inactive producers
                continue;

            int64_t pay_amount = 0;
            index++;
            
            if (index <= 21) {
                pay_amount = (shareValue * int64_t(2));
            } else if (index >= 22 && index <= 51) {
                pay_amount = shareValue;
            } else 
                break;
            
            _gstate.perblock_bucket -= pay_amount;
            _gstate.total_unpaid_blocks -= prod.unpaid_blocks;

            _producers.modify(prod, same_payer, [&](auto &p) {
                p.last_claim_time = ct;
                p.unpaid_blocks = 0;
            });

            auto itr = _payments.find(prod.owner.value);
            
            if (itr == _payments.end()) {
                _payments.emplace(_self, [&]( auto& a ) { 
                    a.bp = prod.owner;
                    a.pay = asset(pay_amount, core_symbol());
                });
            } else //adds new payment to existing payment
                _payments.modify(itr, same_payer, [&]( auto& a ) {
                    a.pay += asset(pay_amount, core_symbol());
                });
        }
    }

} //namespace eosiosystem
