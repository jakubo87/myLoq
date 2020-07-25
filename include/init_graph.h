#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include <iostream>
#include <hwloc.h>
#include <boost/graph/adjacency_list.hpp>
#include <string>
#include <functional>


struct Vertex {
  std::string type; 
  unsigned int index;
};

struct Edge {
  std::string label;
};

using graph_t = boost::adjacency_list<
                     boost::vecS, //  out-edge container selector                  
		                 boost::vecS, //  Vector container selector
                     boost::bidirectionalS, //sets bidirectional edge access
                     Vertex,                //vertex property
                     Edge,                  //edge property (see bundled properties)
                     boost::no_property,    //graph property (bundled properties)
                     boost::multisetS>;     //edge container selector... again? 

graph_t init_graph(const hwloc_topology_t & t);

std::string obj_type_toString(hwloc_obj_t & obj);

double distance(
    const int& vd1,   //source vertex descriptor
    const int& vd2,   //source vertex descriptor
    const graph_t& g, //any graph
    std::function<double(int,int,const graph_t&)> func);


//TODO list:
//querying edges
//make group0 default on init
//make use of dijkstra shortest distance to find shortest path and calculate the distance
//---> going up a "child" means penalty, going down does not add to the penalty
//distances have negative values?
//partitioning group objects. memory?
//partitioning balanced/evenly or with respect to distances in cores
//


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
      {"member"},
      g);
  }
  std::cout << "Group: " << name << " has been created" << std::endl;
  //.. containing vertices ...
  return v;
}

//may need some sort of wildcard that always returns true when compared to predicates
//'static' because otherwise the linker will get troubled... possibly not needed when template<..>
//template<class P, ...> TODO variable number (and order) of predicates
static std::vector<int> get_vds(const graph_t& g, const std::string& t, unsigned int i){
  std::vector<int> res;
  auto range = boost::vertices(g);
    std::for_each(range.first, range.second, [&](const auto & vd)
      {
        if(g[vd].type==t && g[vd].index==i)
          res.push_back(vd);
      });
  return res;
}	


//temporarily
static std::vector<int> get_vds_by_type(const graph_t& g, const std::string& t){
  std::vector<int> res;
  auto range = boost::vertices(g);
    std::for_each(range.first, range.second, [&](const auto & vd)
      {
        if(g[vd].type==t)
          res.push_back(vd);
      });
  return res;
}	
#endif
