#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include <iostream>
#include <hwloc.h>
#include <boost/graph/adjacency_list.hpp>
#include <string>
#include <functional>
#include <tuple>


using VType = std::string;
using EType = std::string;
using Index = unsigned int;


//not accessible via std::get<type>
struct Vertex {
  VType type; 
  Index index;
};

struct Edge {
  EType label;
  double weight; //this weight will probably be updated according to each use of distance function  
};


using graph_t = boost::adjacency_list<
                     boost::vecS, //  out-edge container selector                  
                     boost::vecS, //  Vector container selector
                     boost::bidirectionalS, //sets bidirectional edge access
                     Vertex,                //vertex property
                     Edge,                  //edge property (see bundled properties)
                     boost::no_property,    //graph property (bundled properties)
                     boost::multisetS>;     //edge container selector... again? 

using VD = graph_t::vertex_descriptor;
using ED = graph_t::edge_descriptor;

//using logical_index = decltype(hwloc_get

graph_t init_graph(const hwloc_topology_t & t);


std::string obj_type_toString(hwloc_obj_t & obj);

double distance(
    VD vd1,   //source vertex descriptor
    VD vd2,   //source vertex descriptor
    const graph_t& g, //any graph
    std::function<double(VD,VD,const graph_t&)> func);

//TODO apply_distances()... update the edge weights according to a distamce function.
//globally? locally?
//locally: starting from a point and only those that are reachable... 
//globally: for each vd and all in and out edges.
//question: when to stop if updating causes constant circular updates. graph can have loops after all... probably via an entry in the disjkstra of dfs/bfs visitor

//TODO using auto as a return value in a header is quite lame as it does not show what to expect from the return value...

//the querying interfaces will need a unified version. possibly with strongly typed parameters
auto get_vds(const graph_t& g, const VType& t, Index i);

auto get_vds_by_type(const graph_t& g, const VType& t);

auto get_ed(const graph_t& g, VD va, VD vb, const EType&);

//auto get_ed_property(

//returns path (list of vds and eds) from va to vb in graph g with respect to distance function func
auto shortest_path(const graph_t& g, VD va, VD vb, std::function<double(VD,VD,const graph_t&)> func);

//TODO list:
//querying edges
//partitioning balanced/evenly or with respect to distances in cores





//IMPLEMENTATION

//C is a container from which to get the group's elements' vertex descriptors
//returns the vertex descriptor to the group
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
      {"member", 0.0},
      g);
  }
  std::cout << "Group: " << name << " has been created" << std::endl;
  //.. containing vertices ...
  return v;
}

////check a tuple for its properties (type based)
//// ending case for recursion
//template <typename T>
//inline bool check_props(T && t) {
//  return true;
//}
//// Recursive case
//template <typename T, typename P, typename... Args>
//inline bool check_props(T&& t, P&& p, Args... args) {
//  if (std::get<P>(t)!=p) return false;
//  return check_props(t, args...);
//}
//
////query the vd from properties
//template<typename... Args>
//auto test_get_vds(const graph_t& g, Args... args){
//  std::vector<VD> res;
//  auto range = boost::vertices(g);
//    std::for_each(range.first, range.second, [&](const auto & vd)
//      {
//        auto t = std::make_tuple(g[vd].type,g[vd].index); 
//        if(check_props(t,args...))
//          res.push_back(vd);
//      });
//  return res;
//} 


#endif
