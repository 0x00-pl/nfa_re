#include <algorithm>
#include "regex_64.h"
#include "graph.h"
#include "queue"
using namespace std;
using namespace nfa_re;


static uint64_t ch_mask_64(size_t index, size_t patt_size){
  return 1<<(patt_size-index);
}

static uint64_t build_lexer_data(string& patt){
  uint64_t lexer_data=0;
  bool escape= false;
  for(size_t i=0; i<patt.size(); i++){
    if(!escape && (patt[i]=='(' ||  patt[i]=='|' || patt[i]==')' ||patt[i]=='*')){
      lexer_data|=0;
    }else if(patt[i]=='\\'){
      escape= true;
      lexer_data|=0;
    }else{
      escape= false;
      lexer_data|=1;
    }
    lexer_data<<=1;
  }
  return lexer_data;
}

static void build_nfa(graph& g, string& patt, vector<size_t>& capture_beg, vector<size_t>& capture_end){
  vector<size_t> l_stack;
  size_t last_l= 0;
  bool escape= false;
  for(size_t p=0; p<patt.size(); p++){
    if(!escape && patt[p]=='('){
      g.add_edge(p,p+1);
      l_stack.push_back(p);
      capture_beg.push_back(ch_mask_64(p,patt.size()));
    }else if(!escape && patt[p]=='|'){
      l_stack.push_back(p);
      last_l=0;
      for(int i=l_stack.size()-1; i>0; i--){
	if(patt[l_stack[i]]=='('){ last_l= i; break; }
      }
      g.add_edge(last_l, p+1);
    }else if(!escape && patt[p]==')'){
      g.add_edge(p,p+1);
      capture_end.push_back(ch_mask_64(p,patt.size()));
      while(!l_stack.empty()){
	if(patt[l_stack.back()]=='|'){
	  g.add_edge(l_stack.back(),p);
	  l_stack.pop_back();
	}else if(patt[l_stack.back()]=='('){
	  last_l= l_stack.back();
	  l_stack.pop_back();
	  break; 
	}else{
	  throw "debug unknow";
	}
      }
      // (...)*
      if(p+1<patt.size() && patt[p+1]=='*'){
	g.add_edge(last_l,p+1);
	g.add_edge(p+1,last_l);
      }
    }else if(!escape && patt[p]=='*'){
      g.add_edge(p,p+1);
    }else if(!escape && patt[p]=='\\'){
      g.add_edge(p,p+1);
      escape= true;
    }else{
      // a*
      escape= false;
      if(p+1<patt.size() && patt[p+1]=='*'){
	g.add_edge(p+1,p);
	g.add_edge(p,p+1);
      }
    }
  }
  reverse(capture_end.begin(), capture_end.end());
}

static uint64_t bfs(graph& g, size_t index, size_t patt_size, bool is_forward){
  uint64_t ret=0;
  queue<size_t> q;
  q.push(index);
  while(!q.empty()){
    size_t n= q.front();
    q.pop();
    if(!(ret & ch_mask_64(n,patt_size))){
      ret|= ch_mask_64(n,patt_size);
      vector<size_t>& adj_edge= g._adj(n);
      for(size_t i=0; i<adj_edge.size(); i++){
	if( (g.edges[adj_edge[i]].from==n) == is_forward ){
	  q.push(g.edges[adj_edge[i]].to);
	}else{
	  q.push(g.edges[adj_edge[i]].from);
	}
      }
    }
  }
  return ret;
}

static void build_next_tab(graph& g, size_t patt_size, vector<uint64_t>& next_tab_f, vector<uint64_t>& next_tab_b){
  for(size_t i=0; i<g.v_adj.size(); i++){
    next_tab_f[i]= bfs(g,i,patt_size,true);
  }
  for(size_t i=0; i<g.v_adj.size(); i++){
    next_tab_b[i]= bfs(g,i,patt_size,false);
  }
}

namespace nfa_re{
  
uint64_t regex_64::match_ch(char ch){
  uint64_t ret= 0;
  for(size_t i=0; i<patt.size(); i++){
    if(lexer_data&ch_mask_64(i,patt.size())){
      if(patt[i]=='.' || patt[i]==ch){
	ret|= ch_mask_64(i,patt.size());
      }
    }
  }
  return ret;
}

  
regex_64::regex_64(string _patt)
:next_tab_f(_patt.size()+1,0)
,next_tab_b(_patt.size()+1,0)
,match_tab(256,0){
  if(_patt.size()+1>=64) return;
  patt= _patt;
  lexer_data= build_lexer_data(_patt);
  graph nfag(_patt.size()+1);
  
  build_nfa(nfag, _patt, capture_beg, capture_end);
  
  build_next_tab(nfag, _patt.size(), next_tab_f, next_tab_b);
  
  for(size_t i=0; i<capture_beg.size(); i++){
    capture_beg[i]= e_move(capture_beg[i], true);
  }
  for(size_t i=0; i<capture_end.size(); i++){
    capture_end[i]= e_move(capture_end[i], true);
  }
  for(size_t i=0; i<256; i++){
    match_tab[i]= match_ch(char(i));
  }
}


uint64_t regex_64::e_move(uint64_t state, bool is_forward){
  uint64_t next_state= 0;
  for(size_t i=0; i<patt.size(); i++){
    if(state & ch_mask_64(i,patt.size())){
      next_state|= is_forward? next_tab_f[i]: next_tab_b[i];
    }
  }
  return next_state;
}

uint64_t regex_64::run_step(uint64_t state, char ch, bool is_forward){
    if(is_forward){
    //test ch
    state &= match_tab[ch];
    state >>= 1;

    //next state
    state= e_move(state, is_forward);
  }else{
    //next state
    state= e_move(state, is_forward);
    
    //test ch
    state <<= 1;
    state &= match_tab[ch];
  }
  return state;
}

uint64_t regex_64::run_until_state(uint64_t start_state, uint64_t terminate_state, size_t& ptext_io, string& text, bool is_forward){
  start_state= e_move(start_state, is_forward);
  while( ((start_state&terminate_state)!=terminate_state) && start_state!=0 ){
    start_state= run_step(start_state, text[ptext_io], is_forward);
    ptext_io= is_forward? ptext_io+1: ptext_io-1;
  }
  return start_state;
}
uint64_t regex_64::run_until_ptext(uint64_t start_state, size_t ptext, size_t ptext_dest, string& text, bool is_forward){
  start_state= e_move(start_state, is_forward);
  while(ptext<=text.size() && start_state!=0){
    if(ptext==ptext_dest) {return start_state;}
    start_state= run_step(start_state, text[ptext], is_forward);
    ptext= is_forward? ptext+1: ptext-1;
  }
  return 0;
}

pair<size_t,size_t> regex_64::capture(size_t id, string text){
  size_t pos0= 0;
  size_t pos0b= -1;  
  uint64_t state0= ch_mask_64(0,patt.size());
  uint64_t state0b= 0;
  while(state0!=0 && pos0<text.size()){
    if(state0==state0b && pos0==pos0b){ break; /*dead loop*/}else{state0b=state0;pos0b=pos0;}
    state0= run_until_state(state0, capture_beg[id], pos0, text, true);
    if(state0==0) {break;}
    size_t pos1= pos0;
    size_t pos1b= -1;  
    uint64_t state1= capture_beg[id];
    uint64_t state1b= 0;
    while(state1!=0 && pos1<text.size()){
    if(state1==state1b && pos1==pos1b){ break; /*dead loop*/}else{state1b=state1;pos1b=pos1;}
      state1= run_until_state(state1, capture_end[id], pos1, text, true);
      //if((state1&capture_end[id])!=capture_end[id]){continue;}
      if(state1==0) {break;}
      uint64_t state2= run_until_ptext(capture_end[id], pos1, text.size(), text, true);
      if((state2&1)!=0){
	return pair<size_t,size_t>(pos0, pos1);
      }
    }
  }
  return pair<size_t,size_t>(text.size(),text.size());
}

}
