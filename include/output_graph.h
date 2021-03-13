#ifndef OUTPUT_GRAPH_H
#define OUTPUT_GRAPH_H

#include <iostream>
#include "init_graph.h"
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graphviz.hpp>


//####### Printing the graph
template<typename G>
class label_writer {
public:

  void operator()(std::ostream& out, const VD& v) const {
    out << "[label=\"" << get(&Vertex::type, g, v) << " #" << get(&Vertex::index, g, v) << "\"]";
  }

  void operator()(std::ostream& out, const ED& e) const {
    out << "[label=\"" << get(&Edge::label, g, e) << "\"]";
  }
  const G& g;
};

//void make_dotfile(const graph_t & g, const std::string& dotf = "out.dot");
//void make_dotfile_nolabel(const graph_t & g, const std::string& dotf = "out.dot");
template<typename G>
void make_dotfile(const G& g, const std::string& dotf = "out.dot"){
  std::ofstream dot(dotf);
  label_writer<G> lw{g};   //constructor declaring what graph to use, to use the label_writer as a functor
  boost::write_graphviz(dot, g, lw, lw);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}

template<typename G>
void make_dotfile_nolabel(const G& g, const std::string& dotf = "out.dot"){
  std::ofstream dot(dotf);
  boost::write_graphviz(dot, g);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}



bool is_ancestor(const VD& va, const VD& vb, const graph_t& g); //containment?!

//graph_t make_subgraph(const graph_t& g, const VD& gv);

//making a tree from a group include only the parent/child relations as all other relations are not in the hwloc tree view. possibly other relationships in the future. however for custom relationships it is not guaranteed to have a tree
graph_t make_can_tree(const graph_t& s, const VD& gv, const VType& type);
graph_t make_can_tree(const graph_t& s, const VD& gv);


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
template <typename Map> //will SFINAE make it right? if we use a type, that is only existent in Edge or Vertex
struct constrained_map {
  using res_t = typename boost::property_traits<Map>::value_type;
  using EV    = typename boost::property_traits<Map>::key_type;
  constrained_map(){ }
  constrained_map(Map prop) : m_prop(prop) { }
  bool operator()(const EV ev) const {
    return fun_(get(m_prop, ev));
  }
  //can i potentionally get away with making graph structures instead of ED or VD a constraint???? NO says the documentation...! but maybe... if reduced...to a representative vertex... like a starting point...
  Map m_prop;
  std::function<bool(res_t)> fun_= [&](auto res){return res==0;};
};

//NOTE: maybe just use the graph without & -> by copy, to preserve it and not accidentally write in it...
template<typename P> //NOTE can we just use this for querying literally anything...?
decltype(auto)
filtered_graph(graph_t& g, P p){ // std::function<bool(P)>& fun){ //TODO make it arbitrary in length or leave it to the user.. IDEA make an filtered graph of a filtered graph recursively to facilitate all the needs... otherwise one would have to distinguish which is about vertices and which is about edges
    //std::function fun
  using map_t = decltype(boost::get(p,g));
  constrained_map<map_t> filter(boost::get(p,g)); //cannot deduce, because class...
  return boost::filtered_graph<graph_t, decltype(filter)>(g, filter);
}










template<typename... Args >
graph_t isolate_cathegories(const graph_t& s, Args&&... args){
  graph_t g(s); //TODO filter graph
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
