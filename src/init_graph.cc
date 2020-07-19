#include <iostream>
#include <string>
#include <hwloc.h>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include "../include/init_graph.h"


template<class C>
auto make_group(const std::string & name, const C & cont, graph_t & g){
  //insert group vertex into graph
  auto v = boost::add_vertex(
      {name, //vertex type
      0},     //id - all Groups have id 0 the user needs to choose a unique name
      g);
  for (auto & vd : cont){ //for all vertex descriptors
    //add a connection to the group members
    boost::add_edge(
        v,
        vd,
        {"member"},
        g);
  }
  std::cout << "Group: " << name << " has been created" << std::endl;
  //.. containing vertices ...
  return v;
}


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

 //get all the hw objs

  depth = hwloc_get_type_depth(t, HWLOC_OBJ_PACKAGE);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN) {
  printf("*** The number of packages is unknown\n");
  } else {
  printf("*** %u package(s)\n",hwloc_get_nbobjs_by_depth(t, depth));
  }

  //query all the special types and insert into graph TODO
  //first all the memory for testing
  auto obj = hwloc_get_obj_by_type(t, HWLOC_OBJ_NUMANODE, 0);
  auto nobj = hwloc_get_nbobjs_by_type(t, HWLOC_OBJ_NUMANODE);
  for (int i = 0; i<nobj; ++i){  
    boost::add_vertex(
        {obj_type_toString(obj), //type
         obj->logical_index},    //index
         g); 
    std::cout << "Added vertex: (type: " << obj_type_toString(obj) << ", index: " << obj->logical_index << ")" << std::endl;
  }


  //breadth first core tree traversal -> positive depths
  max_depth = hwloc_topology_get_depth(t);

  for (depth = 0; depth < max_depth; depth++) {
    std::cout << "adding Objects at level to graph: " <<  depth << std::endl;
    for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); ++i) {
      obj = hwloc_get_obj_by_depth(t, depth, i);
      auto v = boost::add_vertex(
         {obj_type_toString(obj), //type
          obj->logical_index},    //index
          g);  
    std::cout << "Added vertex: (type: " << obj_type_toString(obj) << ", index: " << obj->logical_index << ")" << std::endl;
    
    //add relationships
    if (obj->type!=HWLOC_OBJ_MACHINE){
      //from a childs point of view - the parent will have been added already and can be queried. Otherwise it would be necessary to find blank vertices and attach attributes to make them distinguishable  (alternative approach would be a depth first traversal, however parenthood can be over more than one level of depth) maybe TODO
      //question remains how memory plays into this...
      //find all the parents:
      std::vector<int> pa_id;
      std::for_each(vertices(g).first, vertices(g).second,[&](const auto & vd){
        //find out who the parent is
        auto pa_obj = obj->parent;
        if (g[vd].type == obj_type_toString(pa_obj) && g[vd].index == pa_obj->logical_index){
          pa_id.push_back(vd);
	}
      });
      for(auto & p : pa_id){ 
        //Parent / child
        if (boost::add_edge(
        p, //out
        v, //in
        {"child"}, // edge property
        g).second)
          {std::cout << "Added Edge: (from " << p << " to " << v << ", label: child" << std::endl;}
        //TODO check if 2 same edges will exist and make trouble
        }
      }
    }
  }

  //create group of one PU of each core
  //ok.. first an arbitrary group
  std::vector<int> c = {1,2};
  make_group("Group1", c ,g);



/*  //insert relationship among the vertices
  if (boost::add_edge(
    1, //out
    2, //in
    {"test"}, // edge property
    g).second)
    { std::cout << "Added Edge: (from 1 to 2, label: test" << std::endl;}

*/
/* //pick a couple of vertices by type and display properties

  vector<int> cores;  //actually not int but vertex-descriptor which is an alias...
  for (const auto & v boost::vertices(g)){
    if (boost::get(v,,g)){   
      cores.push_back(v);
    }
  };
*/

  //query the type of the first vertex
  //std::cout << boost::get(graph_t::vertex_property_tag::type, g, 1) << std::endl;
  graph_t::vertex_descriptor vd = *vertices(g).first; // first is the begin() iterator dereferncing it will give its index
  std::cout <<"vertex: " << vd << " , type: " << g[vd].type << std::endl;
  //conclusion... querying data is range based.. no big deal.. 

  return g;
}


