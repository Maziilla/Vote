#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
using namespace ::eosio;

namespace voting{
	class votoken: public eosio::contract{
		public:
			using contract::contract;
			votoken(account_name self/*,account_name autor*/):contract(self),_accounts(_self,_self),_posts(_self,_self)
			{
			};
			void getbalance(account_name user);
			void givevote(account_name voter, account_name post);
			void taketoken(account_name user,uint64_t token);
			void creatpost(account_name postid);
struct post{
				//account_name autor;
				uint64_t total_voted = 0;
				//uint64_t max_voted;
				account_name postid;
				uint64_t primary_key()const {return postid;}
				EOSLIB_SERIALIZE(post,(total_voted)(postid))
			};
			eosio::multi_index<N(posts),post> _posts;

		private:
			struct account{
				account_name name;
				account_name proxy;
				uint64_t balance;
			
				uint64_t primary_key()const {return name;}
				EOSLIB_SERIALIZE(account,(name)(proxy)(balance))
			};
			eosio::multi_index<N(accounts),account> _accounts;

			

			struct vote_action {
				account_name	voter;
				account_name	postid;
				uint32_t	votepower;			
			}; 
	};
}
