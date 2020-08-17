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

const double NOPATH = 10.000;

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
                     Edge>;                  //edge property (see bundled properties)
// the following settings seem to contradict some internal details of the adjacency list when using out_edges (hypothesis)
                     //boost::no_property,    //graph property 
                     //boost::multisetS>;     //edge container selector... again? 

using VD = graph_t::vertex_descriptor;
using ED = graph_t::edge_descriptor;

graph_t init_graph(const hwloc_topology_t & t);

std::string obj_type_toString(hwloc_obj_t & obj);

double distance(
    VD vd1,   //source vertex descriptor
    VD vd2,   //source vertex descriptor
    const graph_t& g, //any graph
    std::function<double(VD,VD,const graph_t&)> func);


//the querying interfaces will need a unified version. possibly with strongly typed parameters
std::vector<VD> get_vds(const graph_t& g, const VType& t, Index i);

std::vector<VD> get_vds_by_type(const graph_t& g, const VType& t);

std::vector<ED> get_ed(const graph_t& g, VD va, VD vb, const EType&);

const EType& get_edge_label(const graph_t& g, const ED& ed);

//returns the shortest distance when no direct edge is available, accumulating distances
double shortest_path(const graph_t& g, VD va, VD vb, std::function<double(VD,VD,const graph_t&)> func);

//TODO list:
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
