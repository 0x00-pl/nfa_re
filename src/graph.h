#pragma once
#include <vector>
using namespace std;

namespace nfa_re{
  
  class graph{
  public:
    class edge{
    public:
      size_t from,to;
      edge(size_t _from, size_t _to):from(_from),to(_to){}
    };
    
  public:
    vector< vector<size_t> > v_adj;
    vector<edge> edges;
    
    graph(size_t sum_v):v_adj(sum_v){}
    void add_edge(size_t from, size_t to){
      v_adj[from].push_back(edges.size());
      v_adj[to].push_back(edges.size());
      edges.push_back(edge(from,to));
    }
    
    vector<size_t>& _adj(size_t v){
      return v_adj[v];
    }
  }; 
}


