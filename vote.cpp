#include<vote.hpp>
using namespace voting;

void votoken::getbalance(account_name user){
	auto usr = _accounts.find(user);
	eosio_assert(usr!=_accounts.end(),"This account not exists");
	//const auto& usr = _accounts.get(user);
	print("User:",name{(*usr).name}," Tokens: ",(*usr).balance);
}
void votoken::givevote(account_name user, account_name post){
	auto usr = _accounts.find(user);
	eosio_assert(usr!=_accounts.end(),"This account not exists");
	auto vot_post = _posts.find(post);
	eosio_assert(vot_post!=_posts.end(),"This alterantive not exists");
	eosio_assert((*usr).balance>10,"You don't have enough token for voting");
	_posts.modify(vot_post,0,[&](auto& vot){
		vot.total_voted+=(*usr).balance/10;
	});
	print("Your vote is given to ",name{(*vot_post).postid});

}

void votoken::creatpost(account_name postid){
	auto target = _posts.find(postid);
	eosio_assert(target==_posts.end(),"This alterantive alrady exists");
	_posts.emplace(postid,[&](auto& npost){
		npost.postid=postid;
		npost.total_voted=0;		
	});
	target = _posts.find(postid);
	print("Alternative -> ",name{(*target).postid});
}

void votoken::viewpost(account_name postid)
{
	auto target = _posts.find(postid);
	eosio_assert(target!=_posts.end(),"This alternative not exists");
	print("Alternative: ",name{(*target).postid}," number of votes: ",(*target).total_voted);
	
}

void votoken::taketoken(account_name user,uint64_t token){

	auto account = _accounts.find(user);
	if(account != _accounts.end()){
		
		print("User:",name{(*account).name}, " Tokens: ",(*account).balance," -> ");
		_accounts.modify(account, 0, [&](auto& usr) {usr.balance+=token;});
		print("User:",name{(*account).name}, " Tokens: ",(*account).balance);
	}else{
		_accounts.emplace(user, [&](auto& a) {
		a.name = user;
		a.balance = token;
      	  });	
	}
}
void votoken::view(uint64_t e){
	
	uint64_t k=10;
	uint32_t i=0;
	//auto itr=_posts.begin();
	//print(name{(*itr).postid}," ");
	for( auto& kek : _posts){	
		print(name{kek.postid}," ");
		i++;		
		eosio_assert(i<100,"Looooop");
	print("");
	}
}

EOSIO_ABI(votoken,(getbalance)(taketoken)(creatpost)(givevote)(viewpost)(view))

