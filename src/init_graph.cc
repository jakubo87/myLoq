#include <iostream>
#include <string>
#include <hwloc.h>
#include <vector>
#include <functional>
#include <boost/config.hpp>
#include <boost/array.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/function_property_map.hpp>
#include "../include/init_graph.h"

//this is the interface, which simply uses the distance function.
//it is recommended, to make the distance function not point to a certain graph, but to leave that as a parameter
double distance( 
              VD vd1,
              VD vd2,
              const graph_t& g,  // g is not THE graph, it is any graph included -> shadowing
              std::function<double(VD,VD,const graph_t&)> func){
  return func(vd1, vd2, g);
}


//may need some sort of wildcard that always returns true when compared to predicates
std::vector<VD> get_vds(const graph_t& g, const std::string& t, Index i){
  std::vector<VD> res;  //<- necessary to always have something to return even if nothing matches
  auto range = boost::vertices(g);
    std::for_each(range.first, range.second, [&](const auto & vd)
      {
        if(g[vd].type==t && g[vd].index==i)
          res.push_back(vd);
      });
  return res;
} 

std::vector<VD> get_vds_by_type(const graph_t& g, const std::string& t){
  std::vector<VD> res;
  auto range = boost::vertices(g);
    std::for_each(range.first, range.second, [&](const auto& vd)
      {
        if(g[vd].type==t)
          res.push_back(vd);
      }
    );
  return res;
} 


//query the ed for the edge from va to vb with label (std::string)
std::vector<ED> get_ed(const graph_t& g, VD va, VD vb, const EType& label){
  std::vector<ED> res;
  auto range = boost::edges(g);
  std::for_each(range.first, range.second,[&](const auto& ed)
      {
        if( boost::source(ed,g)==va && 
            boost::target(ed,g)==vb &&
            g[ed].label==label)
          res.push_back(ed);
      }
  );
  return res;
}

//can only be const... or can it..?
const EType& get_edge_label(const graph_t& g, const ED& ed){
  return g[ed].label;
}



//name may be a bit misleading TODO: translation of hwloc_obj->type to string
std::string obj_type_toString(hwloc_obj_t & obj){
  switch (obj->type) {
    case HWLOC_OBJ_MACHINE    :  return "HWLOC_OBJ_MACHINE";
    case HWLOC_OBJ_PACKAGE    :  return "HWLOC_OBJ_PACKAGE";
    case HWLOC_OBJ_CORE       :  return "HWLOC_OBJ_CORE";
    case HWLOC_OBJ_PU         :  return "HWLOC_OBJ_PU";
    case HWLOC_OBJ_L1CACHE    :  return "HWLOC_OBJ_L1CACHE";
    case HWLOC_OBJ_L2CACHE    :  return "HWLOC_OBJ_L2CACHE";
    case HWLOC_OBJ_L3CACHE    :  return "HWLOC_OBJ_L3CACHE";
    case HWLOC_OBJ_L4CACHE    :  return "HWLOC_OBJ_L4CACHE";
    case HWLOC_OBJ_L5CACHE    :  return "HWLOC_OBJ_L5CACHE";
    case HWLOC_OBJ_L1ICACHE   :  return "HWLOC_OBJ_L1ICACHE";
    case HWLOC_OBJ_L2ICACHE   :  return "HWLOC_OBJ_L2ICACHE";
    case HWLOC_OBJ_L3ICACHE   :  return "HWLOC_OBJ_L3ICACHE";
    case HWLOC_OBJ_GROUP      :  return "HWLOC_OBJ_GROUP";
    case HWLOC_OBJ_NUMANODE   :  return "HWLOC_OBJ_NUMANODE";
    case HWLOC_OBJ_BRIDGE     :  return "HWLOC_OBJ_BRIDGE";
    case HWLOC_OBJ_PCI_DEVICE :  return "HWLOC_OBJ_PCI_DEVICE";
    case HWLOC_OBJ_OS_DEVICE  :  return "HWLOC_OBJ_OS_DEVICE";
    case HWLOC_OBJ_MISC       :  return "HWLOC_OBJ_MISC";
    case HWLOC_OBJ_MEMCACHE   :  return "HWLOC_OBJ_MEMCACHE";
    case HWLOC_OBJ_DIE        :  return "HWLOC_OBJ_DIE";
    case HWLOC_OBJ_TYPE_MAX   :  return "HWLOC_OBJ_TYPE_MAX"; //not in docu! TODO
    default                   :  return "HWLOC_CUSTOM_OBJ";
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
    for (Index i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); ++i) {
      auto obj = hwloc_get_obj_by_depth(t, depth, i);
      auto v = boost::add_vertex(
         {obj_type_toString(obj), //type
          obj->logical_index},    //index
          g);  
    std::cout << "Added vertex: (type: " << obj_type_toString(obj) << ", index: " << obj->logical_index << ")" << std::endl;
    
    //add relationships
    if (obj->type!=HWLOC_OBJ_MACHINE){
      //from a childs point of view - the parent will have been added already and can be queried. Otherwise it would be necessary to find blank vertices and attach attributes to make them distinguishable  (alternative approach would be a depth first traversal, however parenthood can be over more than one level of depth) 
      //question remains how memory plays into this...
      //find all the parents:
      auto pa_obj = obj->parent;
      auto pa_id = get_vds(g, obj_type_toString(pa_obj), pa_obj->logical_index);
      for(auto & p : pa_id){ // "for" not necessary in a tree (hwloc) there is only one parent, but maybe aliases...)
        // child
        if (boost::add_edge(
        v, //out
        p, //in
        {"child", 10.0}, // edge property
        g).second)
          {std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;}
        // parent
        if (boost::add_edge(
        p, //out
        v, //in
        {"parent", 0.0}, // edge property
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
    {"child", 10.0}, // edge property
    g).second)
      {std::cout << "Added Edge: (from " << v << " to " << p << ", label: child" << std::endl;}
    // parent
    if (boost::add_edge(
    p, //out
    v, //in
    {"parent", 0.0}, // edge property
    g).second)
      {std::cout << "Added Edge: (from " << p << " to " << v << ", label: parent" << std::endl;}
    //checked if 2 same edges will exist and make trouble -> multiple identical edges can coexist...
    //what is the multiset selector in edge list good for then..? 
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

//Dijkstra
//returns deduced/accumulated distances
//i.e.: if there is no direct connection in a category, hops and penalties of known edges are accumulated along the path. The function is thereby recursive and needs to add a large penalty value if 2 vertices have no direct edge. This is still different from general reachability, which will eventually result from such calculation 
//
//function property needs to heavily penalise edge categories, that are not supposed to be used -> via function, i.e.in the hands of the user. however reachability would not be absolute, only difficult (as in 1e31)
double find_distance(const graph_t& g, VD va, VD vb, std::function<double(VD,VD,const graph_t&)> func){
  std::vector<VD> p(num_vertices(g));
  std::vector<double> d(num_vertices(g));

  //###helper function to get the input right
  std::function<double(ED)> f = [&](const ED& ed)
    {
      auto va = boost::source(ed, g);
      auto vb = boost::target(ed, g); 
      return func(va, vb, g); // by capturing the graph here, you don't need to point to g later
    };

  boost::dijkstra_shortest_paths(
      g,  //graph
      va, //source
      boost::weight_map(boost::function_property_map<decltype(f),ED,double>(f))
      //comment: do NOT(!!) use the make_function_propertymap() function. it fails to deduce correctly!
      .predecessor_map(boost::make_iterator_property_map(
                            p.begin(), get(boost::vertex_index, g)))
      .distance_map(boost::make_iterator_property_map(
                            d.begin(),get(boost::vertex_index, g)))
  );
 
  return d[vb];
}




//Dijkstra
//prints predeccessors from target to source (potentially for debugging..?)
void shortest_path(const graph_t& g, VD va, VD vb, std::function<double(VD,VD,const graph_t&)> func){
  std::vector<VD> directions(num_vertices(g));

  //###helper function to get the input right
  std::function<double(ED)> f = [&](const ED& ed)
    {
      auto va = boost::source(ed, g);
      auto vb = boost::target(ed, g); 
      return func(va, vb, g); // by capturing the graph here, you don't need to point to g later
    };

  boost::dijkstra_shortest_paths(
      g,  //graph
      va, //source
      boost::weight_map(boost::function_property_map<decltype(f),ED,double>(f))
      .predecessor_map(boost::make_iterator_property_map(
                      directions.begin(), get(boost::vertex_index, g))));

  VD p = vb; //target 
  while (p != va) //finish
  {
    std::cout << p << ' ';
    p = directions[p];
  }
  std::cout << p << std::endl;
}
