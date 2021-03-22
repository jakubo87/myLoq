#ifndef OUTPUT_GRAPH_H
#define OUTPUT_GRAPH_H

#include "../include/init_graph.h"
#include "../include/hwloc-test.h"

//####### Printing the graph
template<typename G>
class label_writer {
public:
  void operator()(std::ostream& out, const vd_t& v) const {
    out << "[label=\"" << get(&Vertex::type, g, v) << " #" << get(&Vertex::index, g, v) << "\"]";
  }
  void operator()(std::ostream& out, const ed_t& e) const {
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


void find_pattern(const graph_t& g);

bool is_ancestor(const VD& va, const VD& vb, const graph_t& g); //containment?!


//goto parent iterator -> only in the parent/child cathegory
template<typename G>
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
  const G& g;
  VD vd; //<-pos
};



//###### Implementation
template <typename Map> //will SFINAE make it right? if we use a type, that is only existent in Edge or Vertex
struct constrained_map {
  using res_t = typename boost::property_traits<Map>::value_type;
  constrained_map(){ }
  constrained_map(Map prop) : m_prop(prop) { }
  template<typename EV>
  bool operator()(const EV& ev) const {
    return 0 < boost::get(m_prop, ev);
  }
  Map m_prop;
  //std::function<bool(res_t)> fun_= [&](auto res){return res!=0;};
};

//NOTE: maybe just use the graph without & -> by copy, to preserve it and not accidentally write in it...
template<typename P, typename G, typename EV> //NOTE can we just use this for querying literally anything...?
decltype(auto)
filtered_graph(G& g, P EV::* p){ // std::function<bool(P)>& fun){ //TODO make it arbitrary in length or leave it to the user.. IDEA make an filtered graph of a filtered graph recursively to facilitate all the needs... otherwise one would have to distinguish which is about vertices and which is about edges
    //std::function fun
  using map_t = decltype(boost::get(p,g));
  constrained_map<map_t> filter(boost::get(p,g));
  return boost::filtered_graph<G, constrained_map<map_t>>(g, filter);
}

template<typename P, typename G, typename EV> //NOTE can we just use this for querying literally anything...?
decltype(auto)
filtered_graph(const G& g, P EV::* p){ // std::function<bool(P)>& fun){ //TODO make it arbitrary in length or leave it to the user.. IDEA make an filtered graph of a filtered graph recursively to facilitate all the needs... otherwise one would have to distinguish which is about vertices and which is about edges
    //std::function fun
  using map_t = decltype(boost::get(p,g));
  constrained_map<map_t> filter(boost::get(p,g));
  return boost::filtered_graph<G, constrained_map<map_t>>(g, filter);
}




#endif
