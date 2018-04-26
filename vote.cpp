#include<vote.hpp>
using namespace voting;

void votoken::getbalance(account_name user){
	require_auth(user);	
	auto usr = _accounts.find(user);
	eosio_assert(usr!=_accounts.end(),"This account not exists");
	//const auto& usr = _accounts.get(user);
	print("User:",name{(*usr).name},/*" Proxy:",name{(*usr).proxy},*/"Tokens: ",(*usr).balance);
}
void votoken::subscribe(account_name owner, account_name proxy){
	auto owne = _accounts.find(owner);
	eosio_assert(owne !=_accounts.end(),"Owner not exists");
	auto prox = _accounts.find(proxy);
	eosio_assert(prox!=_accounts.end(),"Proxy not exists");
	eosio_assert((*owne).proxy==NULL,"You alreade subscriby by someone");
	eosio_assert((*prox).proxy!=owner,"You don't break me!");
	((*owne).voted==false,"To subscribe remove an existing vote");
	_accounts.modify(owne,0,[&](auto& user){user.proxy=proxy;});
	_delegated.emplace((proxy,owner),[&](auto& deleg){
		deleg.main=proxy;
		deleg.follow=owner;
	});

	print("You have subscribed to ",name{proxy});
}
void votoken::unsubscribe(account_name owner){
	auto owne = _accounts.find(owner);
	eosio_assert(owne !=_accounts.end(),"Owner not exists");	
	eosio_assert((*owne).proxy!=NULL,"You are not subscriby to anyone");	
	auto del = _delegated.find( (owner,(*owne).proxy) );
	eosio_assert(del!=_delegated.end(),"Oooops");
	_delegated.erase(del);
	print("You unsubscribed from ",name{(*owne).proxy});
	_accounts.modify(owne,0,[&](auto& user){user.proxy=NULL;});
	
	
}
void votoken::givevote(account_name user, account_name post,uint32_t tokens){
	
	auto usr = _accounts.find(user);
	eosio_assert(usr!=_accounts.end(),"This account not exists");
	auto vot_post = _posts.find(post);
	eosio_assert(vot_post!=_posts.end(),"This alterantive not exists");
	eosio_assert((*usr).balance>=tokens,"You want to spend more tokens then you have");
	eosio_assert(tokens>=10,"You don't have enough token for voting");
	auto vot = _vote_action.find((user,post));
	eosio_assert(vot==_vote_action.end(),"You alraedy vote for this altrnative");
	_vote_action.emplace((user,post),[&](auto& nvote){
		nvote.voter=user;
		nvote.postid=post;
		nvote.votepower = tokens/10;		
	});
	_posts.modify(vot_post,0,[&](auto& vot){
		vot.total_voted+=tokens/10;
	});
	_accounts.modify(usr, 0, [&](auto& us) {us.balance-=tokens/10*10;});
	
	print("Your vote is given to ",name{(*vot_post).postid});

}

void votoken::givevot(account_name user, account_name post){
	
	auto usr = _accounts.find(user);
	eosio_assert(usr!=_accounts.end(),"This account not exists");
	auto vot_post = _posts.find(post);
	eosio_assert(vot_post!=_posts.end(),"This alterantive not exists");
	eosio_assert((*usr).balance>=10,"You don't have enough token for voting");//может сломать голос главного при случае, когда у подписавшегося недостаточно средств
	auto vot = _vote_action.find((user,post));
	eosio_assert(vot==_vote_action.end(),"You alraedy vote for this altrnative");
	eosio_assert((*usr).voted==false,"Your vote is given to another alternative");
	_vote_action.emplace((user,post),[&](auto& nvote){
		nvote.voter=user;
		nvote.postid=post;
		nvote.votepower = (*usr).balance/10;		
	});
	_posts.modify(vot_post,0,[&](auto& vot){
		vot.total_voted+=(*usr).balance/10;
	});
	_accounts.modify(usr, 0, [&](auto& us) {
		us.balance-=(*usr).balance/10*10;
		us.voted = true;//delete without delegate
	});
	
	//Delegate
	for( auto& temp : _delegated){	
		if(temp.main == user){
			auto new_temp = _accounts.find(temp.follow);
			givevot((*new_temp).name,post);
		}
	}
	print("Your vote is given to ",name{(*vot_post).postid});

}

void votoken::returnvote(account_name user, account_name post){
	//require_auth(post);
	auto usr = _accounts.find(user);
	eosio_assert(usr!=_accounts.end(),"This account not exists");
	auto vot_post = _posts.find(post);
	eosio_assert(vot_post!=_posts.end(),"This alterantive not exists");
	
	auto vot = _vote_action.find((user,post));
	eosio_assert(vot!=_vote_action.end(),"You not vote for this altrnative");
	_posts.modify(vot_post,0,[&](auto& vote){
		vote.total_voted-=(*vot).votepower;
	});	
	_accounts.modify(usr, 0, [&](auto& us) {
		us.balance+=(*vot).votepower*10;
		us.voted=false;//delete without delegate
	});
	_vote_action.erase(vot);
	//Delegate
	for( auto& temp : _delegated){	
		if(temp.main == user){
			auto new_temp = _accounts.find(temp.follow);
			returnvote((*new_temp).name,post);
		}
	}	
	print("Your vote return from ",name{(*vot_post).postid});

}


void votoken::creatpost(account_name postid){
	auto usr = _accounts.find(_self);
	auto target = _posts.find(postid);
	eosio_assert(target==_posts.end(),"This alterantive alrady exists");
	_posts.emplace(postid,[&](auto& npost){
		npost.postid=postid;
		npost.total_voted=0;
		npost.autor = (*usr).name;		
	});
	target = _posts.find(postid);
	print("Alternative -> ",name{(*target).postid});
}

void votoken::viewpost(account_name postid)
{
	auto target = _posts.find(postid);
	eosio_assert(target!=_posts.end(),"This alternative not exists");
	print("Alternative: ",name{(*target).postid}," number of votes: ",(*target).total_voted," autor: ",name{(*target).autor});
	
}

void votoken::taketoken(account_name user,uint64_t token){
	require_auth(user);
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
	for( auto& kek : _posts){	
		print(name{kek.postid}," ");	
	}
}

void votoken::resultvot(uint64_t e){	
	uint64_t maxvot=0;
	uint32_t count_max=0;
	uint32_t i=0;	
	account_name winner;
	for( auto& kek : _posts){
		if(kek.total_voted > maxvot){
			count_max = 1;
			maxvot = kek.total_voted;
			winner = name{kek.postid};
		}else
			if(kek.total_voted == maxvot)
				count_max++;	
	}
	eosio_assert(count_max==1,"The winner can not be determinate :(");
	print(name{winner});
	returnallvote(1);
}

void votoken::returnallvote(uint64_t e){	
	for( auto& vot : _vote_action){
		auto usr = _accounts.find(name{vot.voter});
		_accounts.modify(usr, 0, [&](auto& us) {us.balance+=vot.votepower*10;});
		auto pos = _posts.find(name{vot.postid});
		_posts.modify(pos, 0, [&](auto& post) {post.total_voted-=vot.votepower;});	
	}	
	while(_vote_action.begin()!=_vote_action.end()){
		_vote_action.erase(_vote_action.begin());		
	}	
	print("Clear");
}
EOSIO_ABI(votoken,(getbalance)(taketoken)(creatpost)(givevote)(viewpost)(view)(returnvote)(resultvot)(returnallvote)(givevot)(subscribe)(unsubscribe))

