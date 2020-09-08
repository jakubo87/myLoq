#ifndef OUTPUT_GRAPH_H
#define OUTPUT_GRAPH_H

#include <iostream>
#include "init_graph.h"

void make_dotfile(const graph_t & g, const std::string& dotf = "out.dot");
void make_dotfile_nolabel(const graph_t & g, const std::string& dotf = "out.dot");

bool is_ancestor(const VD& va, const VD& vb, const graph_t& g); //containment?!

//graph_t make_subgraph(const graph_t& g, const VD& gv);

//making a tree from a group include only the parent/child relations as all other relations are not in the hwloc tree view. possibly other relationships in the future. however for custom relationships it is not guaranteed to have a tree
graph_t make_tree(const graph_t& s, const VD& gv);


//goto parent iterator -> only in the parent/child cathegory
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



//###### Implementation

template<typename... Args >
graph_t isolate_cathegories(const graph_t& s, Args&&... args){
  graph_t g(s); //TODO
return g;
}



//#### check if given ED has any of the cathegories/labels
//template<> //empty case
constexpr
bool check_cathegory(const EType& cat){
  return false;
}
//check if any cathegory matches
template<typename C, typename... Args>
constexpr
bool check_cathegory(const EType& cat,C&& c, Args&&... args ){
  return cat==c || check_cathegory(cat, args...);
}


template<typename... Args>
constexpr
bool is_cathegory(const graph_t& g, ED ed, Args&&... args){
  auto cat = g[ed].label; 
return check_cathegory(cat, args...); 
}

template<typename... Args>
constexpr
std::vector<ED> get_eds(const graph_t& g, Args... args){
  std::vector<ED> res;
  auto range = boost::edges(g);
  std::for_each(range.first, range.second, [&](const auto& ed){
    if (check_cathegory(g[ed].label, args...))
      res.push_back(ed);
    });
  return res;
}



#endif
