#ifndef PREDICATES_H
#define PREDICATES_H

#include "../include/hwloc-test.h"
#include "../include/init_graph.h"

//################# PREDICATES ###################
template<typename T, typename G>
struct VPred {
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  //ctor
 // VPred(const G& g, const T Vertex::* mptr, const T& value): g_(&g), mptr_(mptr), value_(value) {}
  bool operator()(const E) const {return true;}
  bool operator()(const V v) const {
    return boost::get(mptr_,*g_, v) == value_;
  }
  G* g_;
  T Vertex::* mptr_;
  T value_;
};

template<typename T, typename G>
struct EPred {
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  bool operator()(const V) const {return true;}
  bool operator()(const E e) const {
    return boost::get(mptr_,*g_, e) == value_;
  }
  G* g_;
  T Edge::* mptr_;
  T value_;
};

//adjacency
////WARNING can if some far away vertices are in connected by some  lead to false assumptions if not filtered correctly
template<typename G>
struct VPred_adj {
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  //ctor
  bool operator()(const E) const {return true;}
  bool operator()(const V v) const {
    bool res = false;
    if (v==vd) return true;
    auto range = boost::adjacent_vertices(vd,*g_);
    std::for_each(range.first, range.second, [&](auto i){
        if(i == v)
          res = true;
    });
    return res;
  }
  G* g_;
  V vd;
};


//adjacency
////WARNING can if some far away vertices are in connected by some  lead to false assumptions if not filtered correctly
template<typename G>
struct VPred_cat {
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;
  //ctor
  bool operator()(const E) const {return true;}
  bool operator()(const V v) const {
    bool res = false;
    auto o_range = boost::out_edges(v,*g_);
    std::for_each(o_range.first, o_range.second, [&](const auto& e){if (boost::get(&Edge::label, *g_, e) == label_) res = true;});
    auto i_range = boost::in_edges(v,*g_);
    std::for_each(i_range.first, i_range.second, [&](const auto& e){if (boost::get(&Edge::label, *g_, e) == label_) res = true;});
    return res;
  }
  G* g_;
  EType label_;
};

#endif
