#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include <iostream>
#include <hwloc.h>
#include <boost/graph/adjacency_list.hpp>
#include <string>



struct Vertex {
  std::string type; 
  unsigned int index;
};

struct Edge {
  std::string label;
};

using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Vertex, Edge>; //first for test later DiGraph

graph_t init_graph(const hwloc_topology_t & t);



std::string obj_type_toString(hwloc_obj_t & obj);

//C is a container from which to get the groups elements
template<class C>
auto make_group(const std::string & name, const C & cont, graph_t & g);







#endif
