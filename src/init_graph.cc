#include <iostream>
#include <string>
#include <hwloc.h>
#include <boost/graph/adjacency_list.hpp>
#include "../include/init_graph.h"




std::string obj_type_toString(hwloc_obj_t & obj){
  switch (obj->type) {
    case HWLOC_OBJ_MACHINE :  return std::string("HWLOC_OBJ_MACHINE");
    case HWLOC_OBJ_PACKAGE :  return "HWLOC_OBJ_PACKAGE";
    case HWLOC_OBJ_CORE :     return "HWLOC_OBJ_CORE";
    case HWLOC_OBJ_PU :       return "HWLOC_OBJ_PU";
    case HWLOC_OBJ_L1CACHE :  return "HWLOC_OBJ_L1CACHE";
    case HWLOC_OBJ_L2CACHE :  return "HWLOC_OBJ_L2CACHE";
    case HWLOC_OBJ_L3CACHE :  return "HWLOC_OBJ_L3CACHE";
    case HWLOC_OBJ_L4CACHE :  return "HWLOC_OBJ_L4CACHE";
    case HWLOC_OBJ_L5CACHE :  return "HWLOC_OBJ_L5CACHE";
    case HWLOC_OBJ_L1ICACHE : return "HWLOC_OBJ_L1ICACHE";
    case HWLOC_OBJ_L2ICACHE : return "HWLOC_OBJ_L2ICACHE";
    case HWLOC_OBJ_L3ICACHE : return "HWLOC_OBJ_L3ICACHE";
    case HWLOC_OBJ_GROUP : return "HWLOC_OBJ_GROUP";
    case HWLOC_OBJ_NUMANODE : return "HWLOC_OBJ_NUMANODE";
    case HWLOC_OBJ_BRIDGE : return "HWLOC_OBJ_BRIDGE";
    case HWLOC_OBJ_PCI_DEVICE : return "HWLOC_OBJ_PCI_DEVICE";
    case HWLOC_OBJ_OS_DEVICE : return "HWLOC_OBJ_OS_DEVICE";
    case HWLOC_OBJ_MISC : return "HWLOC_OBJ_MISC";
    case HWLOC_OBJ_MEMCACHE : return "HWLOC_OBJ_MEMCACHE";
    case HWLOC_OBJ_DIE : return "HWLOC_OBJ_DIE";
    case HWLOC_OBJ_TYPE_MAX : return "HWLOC_OBJ_TYPE_MAX"; //not in docu! TODO
    default : return "HWLOC_CUSTOM_OBJ";
  };
}

//
//constexpr
//std::string obj_cachetype_toString(hwloc_obj_cache_t & obj){
//  switch (obj->type) {
//    case HWLOC_OBJ_CACHE_UNIFIED :  return std::string("HWLOC_OBJ_CACHE_UNIFIED");
//    case HWLOC_OBJ_CACHE_DATA :  return std::string("HHWLOC_OBJ_CACHE_DATA");
//    case HWLOC_OBJ_CACHE_INSTRUCTION : return std::string("HHWLOC_OBJ_CACHE_DATA");
//    default : return "HWLOC_CUSTOM_CACHE_OBJ"; //??? TODO
//  };
//}
//
//constexpr
//std::string obj_bridgetype_toString(hwloc_obj_cache_t & obj){
//  switch (obj->type) {
//    case HWLOC_OBJ_CACHE_UNIFIED :  return std::string("HWLOC_OBJ_CACHE_UNIFIED");
//    case HWLOC_OBJ_CACHE_DATA :  return std::string("HHWLOC_OBJ_CACHE_DATA");
//    case HWLOC_OBJ_CACHE_INSTRUCTION : return std::string("HHWLOC_OBJ_CACHE_DATA");
//    default : return "HWLOC_CUSTOM_CACHE_OBJ"; //??? TODO
//  };
//}


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


