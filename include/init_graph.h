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

using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Vertex, Edge>; 

graph_t init_graph(const hwloc_topology_t & t);

std::string obj_type_toString(hwloc_obj_t & obj);

double distance(
    const int& vd1,   //source vertex descriptor
    const int& vd2,   //source vertex descriptor
    const graph_t& g, //any graph
    std::function<double(int,int,const graph_t&)> func);

//IMPLEMENTATION

//C is a container from which to get the group's elements
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




#endif
