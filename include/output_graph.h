#ifndef OUTPUT_GRAPH_H
#define OUTPUT_GRAPH_H

#include <iostream>
#include "init_graph.h"

void make_dotfile(const graph_t & g);
void make_dotfile_nolabel(const graph_t & g);

bool is_ancestor(const VD& va, const VD& vb, const graph_t& g); //containment?!

//graph_t make_subgraph(const graph_t& g, const VD& gv);
//graph_t make_tree();

//goto parent iterator
class anc_iterator{
  using self_t = anc_iterator;
  
 public:
  anc_iterator()=delete;
  anc_iterator(const graph_t& g, const VD& vd): g(g), vd(vd){}  //ctor 
  VD operator*() const {return vd;} //this one makes a copy 

  bool operator==(const self_t& rhs) const {return (&g==&rhs.g && vd==rhs.vd);} //comparing identity, not equality of the graph

  //rise operator
  self_t operator++(){
    auto range = boost::out_edges(vd,g);
    auto it = std::find_if(range.first, range.second, [&](const auto& ed){return g[ed].label=="child";});
    if (it!=range.second) //if is someone's child, if not, does nothing
      vd=boost::target(*it,g);
    return *this;
  }
 private:  
  const graph_t& g;
  VD vd; //<-pos
};

#endif
