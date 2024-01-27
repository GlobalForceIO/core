#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/resource_limits_private.hpp>
#include <eosio/chain/transaction_metadata.hpp>
#include <eosio/chain/transaction.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <eosio/chain/database_utils.hpp>
#include <algorithm>

#include <fc/io/raw.hpp>
#include <eosio/chain/asset.hpp>
#include <eosio/chain/name.hpp>
#include <eosio/chain/types.hpp>
#include <eosio/chain/contract_types.hpp>

#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/account_object.hpp>
#include <eosio/chain/code_object.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <boost/container/flat_set.hpp>

#include <eosio/chain/abi_serializer.hpp>
#include <fc/variant_object.hpp>
using namespace eosio::chain;
using namespace fc;

using eosio::chain::name;

namespace eosio { namespace chain { namespace resource_limits {

   abi_serializer abi_ser;
   abi_serializer token_abi_ser;
   
using resource_index_set = index_set<
   resource_limits_index,
   resource_usage_index,
   resource_limits_state_index,
   resource_limits_config_index
>;

static_assert( config::rate_limiting_precision > 0, "config::rate_limiting_precision must be positive" );

static uint64_t update_elastic_limit(uint64_t current_limit, uint64_t average_usage, const elastic_limit_parameters& params) {
   uint64_t result = current_limit;
   if (average_usage > params.target ) {
      result = result * params.contract_rate;
   } else {
      result = result * params.expand_rate;
   }
   return std::min(std::max(result, params.max), params.max * params.max_multiplier);
}

void elastic_limit_parameters::validate()const {
   // At the very least ensure parameters are not set to values that will cause divide by zero errors later on.
   // Stricter checks for sensible values can be added later.
   EOS_ASSERT( periods > 0, resource_limit_exception, "elastic limit parameter 'periods' cannot be zero" );
   EOS_ASSERT( contract_rate.denominator > 0, resource_limit_exception, "elastic limit parameter 'contract_rate' is not a well-defined ratio" );
   EOS_ASSERT( expand_rate.denominator > 0, resource_limit_exception, "elastic limit parameter 'expand_rate' is not a well-defined ratio" );
}


void resource_limits_state_object::update_virtual_cpu_limit( const resource_limits_config_object& cfg ) {
   //idump((average_block_cpu_usage.average()));
   virtual_cpu_limit = update_elastic_limit(virtual_cpu_limit, average_block_cpu_usage.average(), cfg.cpu_limit_parameters);
   //idump((virtual_cpu_limit));
}

void resource_limits_state_object::update_virtual_net_limit( const resource_limits_config_object& cfg ) {
   virtual_net_limit = update_elastic_limit(virtual_net_limit, average_block_net_usage.average(), cfg.net_limit_parameters);
}

void resource_limits_manager::add_indices() {
   resource_index_set::add_indices(_db);
}

void resource_limits_manager::initialize_database() {
   const auto& config = _db.create<resource_limits_config_object>([](resource_limits_config_object& config){
      // see default settings in the declaration
   });

   _db.create<resource_limits_state_object>([&config](resource_limits_state_object& state){
      // see default settings in the declaration

      // start the chain off in a way that it is "congested" aka slow-start
      state.virtual_cpu_limit = config.cpu_limit_parameters.max;
      state.virtual_net_limit = config.net_limit_parameters.max;
   });
}

void resource_limits_manager::add_to_snapshot( const snapshot_writer_ptr& snapshot ) const {
   resource_index_set::walk_indices([this, &snapshot]( auto utils ){
      snapshot->write_section<typename decltype(utils)::index_t::value_type>([this]( auto& section ){
         decltype(utils)::walk(_db, [this, &section]( const auto &row ) {
            section.add_row(row, _db);
         });
      });
   });
}

void resource_limits_manager::read_from_snapshot( const snapshot_reader_ptr& snapshot ) {
   resource_index_set::walk_indices([this, &snapshot]( auto utils ){
      snapshot->read_section<typename decltype(utils)::index_t::value_type>([this]( auto& section ) {
         bool more = !section.empty();
         while(more) {
            decltype(utils)::create(_db, [this, &section, &more]( auto &row ) {
               more = section.read_row(row, _db);
            });
         }
      });
   });
}

//TODO check existing config table
void resource_limits_manager::verify_billtrx_config()const {
	ilog( "ONBILLTRX:: resource_limits_manager: verify_billtrx_config");
	
	account_name code = N(eosio);
	account_name scope = N(eosio);
	account_name tablename = N(configfee);
	
	fc::variant config_fee;
	
	const fc::microseconds abi_serializer_max_time = fc::seconds(10);
	bool  shorten_abi_errors = true;
	const auto& code_account = _db.get<account_object,by_name>( code );
	abi_def abi;
	if( abi_serializer::to_abi(code_account.abi, abi) ) {
		abi_serializer abis( abi, abi_serializer::create_yield_function( abi_serializer_max_time ) );
		const auto* t_id = _db.find<chain::table_id_object, chain::by_code_scope_table>(boost::make_tuple( code, scope, tablename ));
		if (t_id != nullptr) {
			const auto &idx = _db.get_index<key_value_index, by_scope_primary>();
			auto it = idx.find(boost::make_tuple( t_id->id, 0 ));
			if( it != idx.end() ) {
				vector<char> data;
				
				//copy_inline_row(*it, data);
				
				data.resize( it->value.size() );
				memcpy( data.data(), it->value.data(), it->value.size() );
				/*
				static void copy_inline_row(const chain::key_value_object& obj, vector<char>& data) {
					data.resize( obj.value.size() );
					memcpy( data.data(), obj.value.data(), obj.value.size() );
				}
				*/
   
   
				config_fee = abis.binary_to_variant( "config_fee", data, abi_serializer::create_yield_function( abi_serializer_max_time ), shorten_abi_errors );
				
				/*
				if( config_fee.is_object() ) {
				 auto& obj = config_fee.get_object();
				 uint64_t ram_fee = atoi( obj["ram_fee"].as_string() );
				 uint64_t cpu_fee = atoi( obj["cpu_fee"].as_string() );
				 ilog( "ONBILLTRX:: resource_limits_manager: verify_billtrx_config: by_code_scope_table: ram_fee = ${ram_fee} cpu_fee = ${cpu_fee}", ("cpu_fee", ram_fee), ("cpu_fee", cpu_fee));
			  }
				*/
			}
		}
	}
	/*
	auto *config;
    const eosio::chain::table_id_object tbl = _db.get<table_id_object, by_code_scope_table>(boost::make_tuple( code, scope, tablename ));
	//find by primary index, primary = 0
	const auto *obj = _db.find<key_value_object, by_scope_primary>(boost::make_tuple(tbl.id, 0));
	if (obj) {
		fc::datastream<const char *> ds(obj->value.data(), obj->value.size());
		fc::raw::unpack(ds, config);
		
		ilog( "ONBILLTRX:: resource_limits_manager: verify_billtrx_config: by_code_scope_table: ram_fee = ${ram_fee} cpu_fee = ${cpu_fee}", ("cpu_fee", config.ram_fee), ("cpu_fee", config.cpu_fee));
	}
	*/
	
	/*
	const auto& config = _db.get<resource_billtrx_config_object>();
	if(config.cpu_fee <= 0){
		ilog( "ONBILLTRX:: resource_limits_manager: verify_billtrx_config CREATE");
		_db.create<resource_billtrx_config_object>([&]( resource_billtrx_config_object& t ) {
		  t.ram_fee = 10;
		  t.cpu_fee = 10;
		});
	}
	*/
}

//TODO verify billtrx pay
bool resource_limits_manager::verify_billtrx_pay( const account_name& payer, uint64_t cpu, uint64_t ram )const {
	ilog( "ONBILLTRX:: DISABLE verify_billtrx_pay payer: ${payer} cpu: ${cpu} ram: ${ram}", ("payer", payer)("cpu", cpu)("ram", ram));
	/*
	//get config
	const auto& config = _db.get<resource_billtrx_config_object>();
	ilog( "ONBILLTRX:: verify_billtrx_pay config: cpu: ${cpu} ram: ${ram}", ("cpu", config.cpu_fee)("ram", config.ram_fee));
	
	//update config
	_db.modify(config, [&](resource_billtrx_config_object& t){
		t.ram_fee += ram;
		t.cpu_fee += cpu;
	});
	*/
	return true;
}

void resource_limits_manager::initialize_account(const account_name& account) {
   _db.create<resource_limits_object>([&]( resource_limits_object& bl ) {
      bl.owner = account;
   });

   _db.create<resource_usage_object>([&]( resource_usage_object& bu ) {
      bu.owner = account;
   });
}

void resource_limits_manager::set_block_parameters(const elastic_limit_parameters& cpu_limit_parameters, const elastic_limit_parameters& net_limit_parameters ) {
   cpu_limit_parameters.validate();
   net_limit_parameters.validate();
   const auto& config = _db.get<resource_limits_config_object>();
   if( config.cpu_limit_parameters == cpu_limit_parameters && config.net_limit_parameters == net_limit_parameters )
      return;
   _db.modify(config, [&](resource_limits_config_object& c){
      c.cpu_limit_parameters = cpu_limit_parameters;
      c.net_limit_parameters = net_limit_parameters;
   });
}

void resource_limits_manager::update_account_usage(const flat_set<account_name>& accounts, uint32_t time_slot ) {
   //TODO remove limit resources for account
   /*
   const auto& config = _db.get<resource_limits_config_object>();
   for( const auto& a : accounts ) {
      const auto& usage = _db.get<resource_usage_object,by_owner>( a );
      _db.modify( usage, [&]( auto& bu ){
          bu.net_usage.add( 0, time_slot, config.account_net_usage_average_window );
          bu.cpu_usage.add( 0, time_slot, config.account_cpu_usage_average_window );
      });
   }
   */
}

void resource_limits_manager::add_transaction_usage(const flat_set<account_name>& accounts, uint64_t cpu_usage, uint64_t net_usage, uint32_t time_slot ) {
   const auto& state = _db.get<resource_limits_state_object>();
   const auto& config = _db.get<resource_limits_config_object>();
	
	//TODO remove limit resources for account
	/*
   for( const auto& a : accounts ) {

      const auto& usage = _db.get<resource_usage_object,by_owner>( a );
      int64_t unused;
      int64_t net_weight;
      int64_t cpu_weight;
      get_account_limits( a, unused, net_weight, cpu_weight );

      _db.modify( usage, [&]( auto& bu ){
          bu.net_usage.add( net_usage, time_slot, config.account_net_usage_average_window );
          bu.cpu_usage.add( cpu_usage, time_slot, config.account_cpu_usage_average_window );
      });

      if( cpu_weight >= 0 && state.total_cpu_weight > 0 ) {
         uint128_t window_size = config.account_cpu_usage_average_window;
         auto virtual_network_capacity_in_window = (uint128_t)state.virtual_cpu_limit * window_size;
         auto cpu_used_in_window                 = ((uint128_t)usage.cpu_usage.value_ex * window_size) / (uint128_t)config::rate_limiting_precision;

         uint128_t user_weight     = (uint128_t)cpu_weight;
         uint128_t all_user_weight = state.total_cpu_weight;

         auto max_user_use_in_window = (virtual_network_capacity_in_window * user_weight) / all_user_weight;

         EOS_ASSERT( cpu_used_in_window <= max_user_use_in_window,
                     tx_cpu_usage_exceeded,
                     "authorizing account '${n}' has insufficient cpu resources for this transaction",
                     ("n", name(a))
                     ("cpu_used_in_window",cpu_used_in_window)
                     ("max_user_use_in_window",max_user_use_in_window) );
      }

      if( net_weight >= 0 && state.total_net_weight > 0) {

         uint128_t window_size = config.account_net_usage_average_window;
         auto virtual_network_capacity_in_window = (uint128_t)state.virtual_net_limit * window_size;
         auto net_used_in_window                 = ((uint128_t)usage.net_usage.value_ex * window_size) / (uint128_t)config::rate_limiting_precision;

         uint128_t user_weight     = (uint128_t)net_weight;
         uint128_t all_user_weight = state.total_net_weight;

         auto max_user_use_in_window = (virtual_network_capacity_in_window * user_weight) / all_user_weight;

         EOS_ASSERT( net_used_in_window <= max_user_use_in_window,
                     tx_net_usage_exceeded,
                     "authorizing account '${n}' has insufficient net resources for this transaction",
                     ("n", name(a))
                     ("net_used_in_window",net_used_in_window)
                     ("max_user_use_in_window",max_user_use_in_window) );

      }
   }
	*/
	
   //TODO leave total used resources bot block
   // account for this transaction in the block and do not exceed those limits either
   _db.modify(state, [&](resource_limits_state_object& rls){
      rls.pending_cpu_usage += cpu_usage;
      rls.pending_net_usage += net_usage;
   });

   EOS_ASSERT( state.pending_cpu_usage <= config.cpu_limit_parameters.max, block_resource_exhausted, "Block has insufficient cpu resources" );
   EOS_ASSERT( state.pending_net_usage <= config.net_limit_parameters.max, block_resource_exhausted, "Block has insufficient net resources" );
}
	  
void resource_limits_manager::add_pending_ram_usage( const account_name account, int64_t ram_delta ) {
   if (ram_delta == 0) {
      return;
   }

   const auto& usage  = _db.get<resource_usage_object,by_owner>( account );

   EOS_ASSERT( ram_delta <= 0 || UINT64_MAX - usage.ram_usage >= (uint64_t)ram_delta, transaction_exception,
              "Ram usage delta would overflow UINT64_MAX");
   EOS_ASSERT(ram_delta >= 0 || usage.ram_usage >= (uint64_t)(-ram_delta), transaction_exception,
              "Ram usage delta would underflow UINT64_MAX");

   _db.modify( usage, [&]( auto& u ) {
     u.ram_usage += ram_delta;
   });
}

//TODO remove limit resources for account
void resource_limits_manager::verify_account_ram_usage( const account_name account )const {
	/*
	int64_t ram_bytes; int64_t net_weight; int64_t cpu_weight;
   get_account_limits( account, ram_bytes, net_weight, cpu_weight );
   const auto& usage  = _db.get<resource_usage_object,by_owner>( account );

   if( ram_bytes >= 0 ) {
      EOS_ASSERT( usage.ram_usage <= static_cast<uint64_t>(ram_bytes), ram_usage_exceeded,
                  "account ${account} has insufficient ram; needs ${needs} bytes has ${available} bytes!",
                  ("account", account)("needs",usage.ram_usage)("available",ram_bytes)              );
   }
   */
   //TEST RAM CALC
   //payment for RAM resources
   //const auto& usage  = _db.get<resource_usage_object,by_owner>( account );
   //EOS_ASSERT( false, ram_usage_exceeded, "RAM ${ramb} bytes", ("ramb",usage.ram_usage));
}

//TODO remove get balance for account
uint64_t resource_limits_manager::check_payment_balance( const account_name account, chain::symbol token )const {
	share_type balance = 0;
    const eosio::chain::table_id_object tbl = _db.get<table_id_object, by_code_scope_table>(boost::make_tuple(N(eosio.token), account, N(accounts)));
	const auto *obj = _db.find<key_value_object, by_scope_primary>(boost::make_tuple(tbl.id, token.to_symbol_code().value));
	if (obj) {
		fc::datastream<const char *> ds(obj->value.data(), obj->value.size());
		fc::raw::unpack(ds, balance);
	}
	return balance;
}

int64_t resource_limits_manager::get_account_ram_usage( const account_name& name )const {
   return _db.get<resource_usage_object,by_owner>( name ).ram_usage;
}

int64_t resource_limits_manager::get_fee_cpu()const {
   return 22;
}

int64_t resource_limits_manager::get_fee_ram()const {
   return 21;
}

bool resource_limits_manager::set_account_limits( const account_name& account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight) {
	//TODO remove limit resources for account
	return false;
   
   //const auto& usage = _db.get<resource_usage_object,by_owner>( account );
   /*
    * Since we need to delay these until the next resource limiting boundary, these are created in a "pending"
    * state or adjusted in an existing "pending" state.  The chain controller will collapse "pending" state into
    * the actual state at the next appropriate boundary.
    */
   auto find_or_create_pending_limits = [&]() -> const resource_limits_object& {
      const auto* pending_limits = _db.find<resource_limits_object, by_owner>( boost::make_tuple(true, account) );
      if (pending_limits == nullptr) {
         const auto& limits = _db.get<resource_limits_object, by_owner>( boost::make_tuple(false, account));
         return _db.create<resource_limits_object>([&](resource_limits_object& pending_limits){
            pending_limits.owner = limits.owner;
            pending_limits.ram_bytes = limits.ram_bytes;
            pending_limits.net_weight = limits.net_weight;
            pending_limits.cpu_weight = limits.cpu_weight;
            pending_limits.pending = true;
         });
      } else {
         return *pending_limits;
      }
   };

   // update the users weights directly
   auto& limits = find_or_create_pending_limits();

   bool decreased_limit = false;

   if( ram_bytes >= 0 ) {

      decreased_limit = ( (limits.ram_bytes < 0) || (ram_bytes < limits.ram_bytes) );

      /*
      if( limits.ram_bytes < 0 ) {
         EOS_ASSERT(ram_bytes >= usage.ram_usage, wasm_execution_error, "converting unlimited account would result in overcommitment [commit=${c}, desired limit=${l}]", ("c", usage.ram_usage)("l", ram_bytes));
      } else {
         EOS_ASSERT(ram_bytes >= usage.ram_usage, wasm_execution_error, "attempting to release committed ram resources [commit=${c}, desired limit=${l}]", ("c", usage.ram_usage)("l", ram_bytes));
      }
      */
   }

   _db.modify( limits, [&]( resource_limits_object& pending_limits ){
      pending_limits.ram_bytes = ram_bytes;
      pending_limits.net_weight = net_weight;
      pending_limits.cpu_weight = cpu_weight;
   });

   return decreased_limit;
}

void resource_limits_manager::get_account_limits( const account_name& account, int64_t& ram_bytes, int64_t& net_weight, int64_t& cpu_weight ) const {
   const auto* pending_buo = _db.find<resource_limits_object,by_owner>( boost::make_tuple(true, account) );
   if (pending_buo) {
      ram_bytes  = pending_buo->ram_bytes;
      net_weight = pending_buo->net_weight;
      cpu_weight = pending_buo->cpu_weight;
   } else {
      const auto& buo = _db.get<resource_limits_object,by_owner>( boost::make_tuple( false, account ) );
      ram_bytes  = buo.ram_bytes;
      net_weight = buo.net_weight;
      cpu_weight = buo.cpu_weight;
   }
}

bool resource_limits_manager::is_unlimited_cpu( const account_name& account ) const {
   const auto* buo = _db.find<resource_limits_object,by_owner>( boost::make_tuple(false, account) );
   if (buo) {
      return buo->cpu_weight == -1;
   }
   return false;
}

void resource_limits_manager::process_account_limit_updates() {
   auto& multi_index = _db.get_mutable_index<resource_limits_index>();
   auto& by_owner_index = multi_index.indices().get<by_owner>();

   // convenience local lambda to reduce clutter
   auto update_state_and_value = [](uint64_t &total, int64_t &value, int64_t pending_value, const char* debug_which) -> void {
      if (value > 0) {
         EOS_ASSERT(total >= static_cast<uint64_t>(value), rate_limiting_state_inconsistent, "underflow when reverting old value to ${which}", ("which", debug_which));
         total -= value;
      }

      if (pending_value > 0) {
         EOS_ASSERT(UINT64_MAX - total >= static_cast<uint64_t>(pending_value), rate_limiting_state_inconsistent, "overflow when applying new value to ${which}", ("which", debug_which));
         total += pending_value;
      }

      value = pending_value;
   };

   const auto& state = _db.get<resource_limits_state_object>();
   _db.modify(state, [&](resource_limits_state_object& rso){
      while(!by_owner_index.empty()) {
         const auto& itr = by_owner_index.lower_bound(boost::make_tuple(true));
         if (itr == by_owner_index.end() || itr->pending!= true) {
            break;
         }

         const auto& actual_entry = _db.get<resource_limits_object, by_owner>(boost::make_tuple(false, itr->owner));
         _db.modify(actual_entry, [&](resource_limits_object& rlo){
            update_state_and_value(rso.total_ram_bytes,  rlo.ram_bytes,  itr->ram_bytes, "ram_bytes");
            update_state_and_value(rso.total_cpu_weight, rlo.cpu_weight, itr->cpu_weight, "cpu_weight");
            update_state_and_value(rso.total_net_weight, rlo.net_weight, itr->net_weight, "net_weight");
         });

         multi_index.remove(*itr);
      }
   });
}

void resource_limits_manager::process_block_usage(uint32_t block_num) {
   const auto& s = _db.get<resource_limits_state_object>();
   const auto& config = _db.get<resource_limits_config_object>();
   _db.modify(s, [&](resource_limits_state_object& state){
      // apply pending usage, update virtual limits and reset the pending

      state.average_block_cpu_usage.add(state.pending_cpu_usage, block_num, config.cpu_limit_parameters.periods);
      state.update_virtual_cpu_limit(config);
      state.pending_cpu_usage = 0;

      state.average_block_net_usage.add(state.pending_net_usage, block_num, config.net_limit_parameters.periods);
      state.update_virtual_net_limit(config);
      state.pending_net_usage = 0;

   });

}

uint64_t resource_limits_manager::get_virtual_block_cpu_limit() const {
   const auto& state = _db.get<resource_limits_state_object>();
   return state.virtual_cpu_limit;
}

uint64_t resource_limits_manager::get_virtual_block_net_limit() const {
   const auto& state = _db.get<resource_limits_state_object>();
   return state.virtual_net_limit;
}

uint64_t resource_limits_manager::get_block_cpu_limit() const {
   const auto& state = _db.get<resource_limits_state_object>();
   const auto& config = _db.get<resource_limits_config_object>();
   return config.cpu_limit_parameters.max - state.pending_cpu_usage;
}

uint64_t resource_limits_manager::get_block_net_limit() const {
   const auto& state = _db.get<resource_limits_state_object>();
   const auto& config = _db.get<resource_limits_config_object>();
   return config.net_limit_parameters.max - state.pending_net_usage;
}

std::pair<int64_t, bool> resource_limits_manager::get_account_cpu_limit( const account_name& name, uint32_t greylist_limit ) const {
   auto [arl, greylisted] = get_account_cpu_limit_ex(name, greylist_limit);
   return {arl.available, greylisted};
}

std::pair<account_resource_limit, bool> resource_limits_manager::get_account_cpu_limit_ex( const account_name& name, uint32_t greylist_limit ) const {
	//TODO remove limit CPU resources for account
	return {{ -1, -1, -1 }, false};

   const auto& state = _db.get<resource_limits_state_object>();
   const auto& usage = _db.get<resource_usage_object, by_owner>(name);
   const auto& config = _db.get<resource_limits_config_object>();

   int64_t cpu_weight, x, y;
   get_account_limits( name, x, y, cpu_weight );

   if( cpu_weight < 0 || state.total_cpu_weight == 0 ) {
      return {{ -1, -1, -1 }, false};
   }

   account_resource_limit arl;

   uint128_t window_size = config.account_cpu_usage_average_window;

   bool greylisted = false;
   uint128_t virtual_cpu_capacity_in_window = window_size;
   if( greylist_limit < config::maximum_elastic_resource_multiplier ) {
      uint64_t greylisted_virtual_cpu_limit = config.cpu_limit_parameters.max * greylist_limit;
      if( greylisted_virtual_cpu_limit < state.virtual_cpu_limit ) {
         virtual_cpu_capacity_in_window *= greylisted_virtual_cpu_limit;
         greylisted = true;
      } else {
         virtual_cpu_capacity_in_window *= state.virtual_cpu_limit;
      }
   } else {
      virtual_cpu_capacity_in_window *= state.virtual_cpu_limit;
   }

   uint128_t user_weight     = (uint128_t)cpu_weight;
   uint128_t all_user_weight = (uint128_t)state.total_cpu_weight;

   auto max_user_use_in_window = (virtual_cpu_capacity_in_window * user_weight) / all_user_weight;
   auto cpu_used_in_window  = impl::integer_divide_ceil((uint128_t)usage.cpu_usage.value_ex * window_size, (uint128_t)config::rate_limiting_precision);

   if( max_user_use_in_window <= cpu_used_in_window )
      arl.available = 0;
   else
      arl.available = impl::downgrade_cast<int64_t>(max_user_use_in_window - cpu_used_in_window);

   arl.used = impl::downgrade_cast<int64_t>(cpu_used_in_window);
   arl.max = impl::downgrade_cast<int64_t>(max_user_use_in_window);
   return {arl, greylisted};
}

std::pair<int64_t, bool> resource_limits_manager::get_account_net_limit( const account_name& name, uint32_t greylist_limit ) const {
   auto [arl, greylisted] = get_account_net_limit_ex(name, greylist_limit);
   return {arl.available, greylisted};
}

std::pair<account_resource_limit, bool> resource_limits_manager::get_account_net_limit_ex( const account_name& name, uint32_t greylist_limit ) const {
	//TODO remove limit NET resources for account
	return {{ -1, -1, -1 }, false};
	
   const auto& config = _db.get<resource_limits_config_object>();
   const auto& state  = _db.get<resource_limits_state_object>();
   const auto& usage  = _db.get<resource_usage_object, by_owner>(name);

   int64_t net_weight, x, y;
   get_account_limits( name, x, net_weight, y );

   if( net_weight < 0 || state.total_net_weight == 0) {
      return {{ -1, -1, -1 }, false};
   }

   account_resource_limit arl;

   uint128_t window_size = config.account_net_usage_average_window;

   bool greylisted = false;
   uint128_t virtual_network_capacity_in_window = window_size;
   if( greylist_limit < config::maximum_elastic_resource_multiplier ) {
      uint64_t greylisted_virtual_net_limit = config.net_limit_parameters.max * greylist_limit;
      if( greylisted_virtual_net_limit < state.virtual_net_limit ) {
         virtual_network_capacity_in_window *= greylisted_virtual_net_limit;
         greylisted = true;
      } else {
         virtual_network_capacity_in_window *= state.virtual_net_limit;
      }
   } else {
      virtual_network_capacity_in_window *= state.virtual_net_limit;
   }

   uint128_t user_weight     = (uint128_t)net_weight;
   uint128_t all_user_weight = (uint128_t)state.total_net_weight;

   auto max_user_use_in_window = (virtual_network_capacity_in_window * user_weight) / all_user_weight;
   auto net_used_in_window  = impl::integer_divide_ceil((uint128_t)usage.net_usage.value_ex * window_size, (uint128_t)config::rate_limiting_precision);

   if( max_user_use_in_window <= net_used_in_window )
      arl.available = 0;
   else
      arl.available = impl::downgrade_cast<int64_t>(max_user_use_in_window - net_used_in_window);

   arl.used = impl::downgrade_cast<int64_t>(net_used_in_window);
   arl.max = impl::downgrade_cast<int64_t>(max_user_use_in_window);
   return {arl, greylisted};
}

} } } /// eosio::chain::resource_limits
