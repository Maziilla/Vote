#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
using namespace ::eosio;

namespace voting{
	class votoken: public eosio::contract{
		public:
			using contract::contract;
			votoken(account_name self/*,account_name autor*/):contract(self),_accounts(_self,_self),_posts(_self,_self),_vote_action(_self,_self),_delegated(_self,_self)
			{
			};
			void getbalance(account_name user);
			void givevote(account_name voter, account_name post,uint32_t);
			void taketoken(account_name user,uint64_t token);
			void creatpost(account_name postid);
			void viewpost(account_name postid);
			void view(uint64_t e);
			void returnvote(account_name voter, account_name post);
			void transfer(account_name from, account_name to);
			void resultvot(uint64_t e);
			void returnallvote(uint64_t e);
			void givevot(account_name voter, account_name post);
			void subscribe(account_name owner, account_name proxy);
			void unsubscribe(account_name owner);
			void viewvoted(uint64_t e);
			void viewdeleg(uint64_t e);
		private:
			struct account{
				account_name name;
				account_name proxy;
				bool voted;
				uint64_t balance;
			
				uint64_t primary_key()const {return name;}
				EOSLIB_SERIALIZE(account,(name)(proxy)(balance))
			};
			eosio::multi_index<N(accounts),account> _accounts;

			struct post{
				account_name autor;
				uint64_t total_voted = 0;
				//uint64_t max_voted;
				account_name postid;
				uint64_t primary_key()const {return postid;}
				EOSLIB_SERIALIZE(post,(total_voted)(postid)(autor))
			};
			eosio::multi_index<N(posts),post> _posts;


			struct vote_action {
				account_name	voter;
				account_name	postid;
				//bool 		voited;
				uint32_t	votepower;	
				uint64_t primary_key()const {return (postid,voter);}
				EOSLIB_SERIALIZE(vote_action,(voter)(postid)(votepower))		
			}; 
			eosio::multi_index<N(vote_actions),vote_action> _vote_action;

			struct delegated{
				account_name	main;
				account_name	follow;
				uint64_t primary_key()const {return (main,follow);}
				EOSLIB_SERIALIZE(delegated,(main)(follow))
			};
			eosio::multi_index<N(delegateds),delegated> _delegated;

	};
}
