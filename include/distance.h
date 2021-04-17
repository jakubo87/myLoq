#ifndef DISTANCE_H
#define DISTANCE_H

#include "../include/init_graph.h"
#include "../include/output_graph.h"
#include <cmath>

//calculates distance by maxlevel to cross for communication
struct lca_dist_s{
  template<typename G, typename V>
  double operator()(V va, V vb, const G& g){
    return pow(10.0, 10 - boost::get(&Vertex::depth, g, lca(g,va,vb)));
  }
};

std::function<double(VD, VD, const graph_t&)> lca_dist = lca_dist_s();


#endif
