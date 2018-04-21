#include<vote.hpp>
using namespace voting;

void votoken::getbalance(account_name user){
	auto usr = _accounts.find(user);
	eosio_assert(usr!=_accounts.end(),"Ooops");
	//const auto& usr = _accounts.get(user);
	print("User:",name{(*usr).name}," Tokens: ",(*usr).balance);
}
void votoken::givevote(account_name user, account_name post){
	
}
void creatpost(account_name postid){
	auto target = _posts.find(postid);
	eosio_assert(target!=_posts.end(),"This alterantive alrady exists")
	_posts.emplace(postid,[&](auto& npost){
		npost.postid=postid;
		npost.total_voted=0;
	});
}
void votoken::taketoken(account_name user,uint64_t token){

	auto account = _accounts.find(user);
	if(account != _accounts.end()){
		
		print("User:",name{(*account).name}, " Tokens: ",(*account).balance," -> ");
		_accounts.modify(account, 0, [&](auto& usr) {usr.balance+=token;});
		print(" User:",name{(*account).name}, " Tokens: ",(*account).balance);
	}else{
		_accounts.emplace(user, [&](auto& a) {
		a.name = user;
		a.balance = token;
      	  });	
	}
}

EOSIO_ABI(votoken,(getbalance)(taketoken)(creatpost))

