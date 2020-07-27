#include <iostream>
#include <string>
#include <hwloc.h>
#include <vector>
#include <functional>
#include <boost/graph/adjacency_list.hpp>
#include "../include/init_graph.h"

//come to think of it...
//it's pretty pointless to have a user define a distance, and then reaping all the credits for providing means of using it yourself...
double distance( // g is not THE graph, it is any graph included -> shadowing
              const VD& vd1,
              const VD& vd2,
              const graph_t& g,
              std::function<double(VD,VD,const graph_t&)> func){
  return func(vd1, vd2, g);
}


//may need some sort of wildcard that always returns true when compared to predicates
//'static' because otherwise the linker will get troubled... possibly not needed when template<..>
auto get_vds(const graph_t& g, const std::string& t, unsigned int i){
  std::vector<VD> res;
  auto range = boost::vertices(g);
    std::for_each(range.first, range.second, [&](const auto & vd)
      {
        if(g[vd].type==t && g[vd].index==i)
          res.push_back(vd);
      });
  return res;
}	

auto get_vds_by_type(const graph_t& g, const std::string& t){
  std::vector<unsigned VD> res;
  auto range = boost::vertices(g);
    std::for_each(range.first, range.second, [&](const auto & vd)
      {
        if(g[vd].type==t)
          res.push_back(vd);
      });
  return res;
}	



//name may be a bit misleading TODO: translation of hwloc_obj->type to string
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
  auto nobj = hwloc_get_nbobjs_by_type(t, HWLOC_OBJ_NUMANODE);
  for (int i = 0; i<nobj; ++i){  
    auto obj = hwloc_get_obj_by_type(t, HWLOC_OBJ_NUMANODE, i);
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
      auto obj = hwloc_get_obj_by_depth(t, depth, i);
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
      for(auto & p : pa_id){ // for not necessary as in a tree (hwloc) there is only one parent)
        // child
        if (boost::add_edge(
        v, //out
        p, //in
        {"child"}, // edge property
        g).second)
          {std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;}
        // parent
        if (boost::add_edge(
        p, //out
        v, //in
        {"parent"}, // edge property
        g).second)
          {std::cout << "Added Edge: (from " << p << " to " << v << ", label: parent" << std::endl;}
        //checked if 2 same edges will exist and make trouble -> multiple identical edges can coexist...
        //what is the multiset selector in edge list good for them..? TODO
        }
      }
    }
  }

  //memory relationships - Adding here to not query vertices that have not been established yet
  nobj = hwloc_get_nbobjs_by_type(t, HWLOC_OBJ_NUMANODE);
  for (int i = 0; i<nobj; ++i){
    auto obj = hwloc_get_obj_by_type(t, HWLOC_OBJ_NUMANODE, i);
    auto v = get_vds(g, obj_type_toString(obj), obj->logical_index).at(0); 
    //query parent vertex - again only go from child nodes to parents
    auto pobj = obj->parent;
    auto p = get_vds(g, obj_type_toString(pobj), pobj->logical_index).at(0); 

    // child
    if (boost::add_edge( // the "if" may be unnecessary, because the container allows for parallel edges 
    v, //out - the numa node vertices are being constructed first, so their vd is the index i 
    p, //in
    {"child"}, // edge property
    g).second)
      {std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;}
    // parent
    if (boost::add_edge(
    p, //out
    v, //in
    {"parent"}, // edge property
    g).second)
      {std::cout << "Added Edge: (from " << p << " to " << v << ", label: parent" << std::endl;}
    //checked if 2 same edges will exist and make trouble -> multiple identical edges can coexist...
    //what is the multiset selector in edge list good for them..? TODO
  }

  //make default group with all memory and all cores
  auto grp = get_vds_by_type(g,"HWLOC_OBJ_CORE");
  auto mem = get_vds_by_type(g,"HWLOC_OBJ_NUMANODE");

 // auto grp = get_vds(g,"HWLOC_OBJ_CORE");
 // auto mem = get_vds(g,"HWLOC_OBJ_NUMANODE");

  for (const auto& i : mem)
    grp.push_back(i);
  make_group("Group0", grp, g);

  return g;
}


////little helper
////container that returns a distance value (so only readable) 
//
//
//
//
////Dijkstra
////returns path (list of vds and eds) from va to vb in graph g with respect to distance function fun
//std::vector<VD> shortest_path(const graph_t& g, VD va, VD vb, std::function<double(VD,VD,graph_t)> func){
//  vector<double> distances(num_vertices(g));
//  dijkstra_shortest_paths(
//      g,  //graph
//      va, //source
//     // weight_map(&edge::distance,g).distance_map( 
//     //     make_iterator_property_map(
//     //         distances.begin(),
//     //         get(vertex_index, map) 
//     //     )
//     // )
//      
//  );
////should we make a distance matrix with respect to a distance function?
////functions need to choose edge types
////... or their own graph altogether...
//
//}

