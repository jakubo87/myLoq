#include <iostream>
#include <hwloc.h>
#include <boost/graph/adjacency_list.hpp>
#include "../include/init_graph.h"
#include "main.cc"

graph_t init_graph(const hwloc_topology_t & t){

graph_t g;
  int max_depth=0;
  int depth=0;

  //breadth first
  std::cout  << "breadth first tree translation" << std::endl;

  max_depth = hwloc_topology_get_depth(t);

  for (depth = 0; depth < max_depth; depth++) {
    std::cout << "adding Objects at level to graph: " <<  depth << std::endl;
    for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
      auto hw_obj = hwloc_get_obj_by_depth(t, depth, i);
//    s=obj_type_toString(hw_obj) + " " + std::to_string(i) + "\n";
//    s+= "OS_index: " + std::to_string(hw_obj->os_index) + "??? \n";
//    s+= "logical index: " + std::to_string(hw_obj->logical_index);
//    s+='\n';
// 

      //TODO  get memory
      //filter by level add edges, add weights, print .dot file 
      //make directed


      //auto v = boost::add_vertex({obj_type_toString(hw_obj) + std::to_string(hw_obj->logical_index), depth}, g);  
      auto v = boost::add_vertex(g);  
      std::cout << boost::num_vertices(g) << std::endl;
    }
  }
  return g;
}


