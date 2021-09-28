#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include "../include/hwloc-test.h"
#include <boost/graph/adjacency_list.hpp>

using VType = std::string;
using Index = unsigned int;
using Mem   = unsigned long int;
using SIMD  = unsigned int;
using RAND  = std::string;
using VID   = unsigned long int; //eventually tags... for identifying vertices across  graphs
using Depth = int;

using EType = std::string;
using EID    = unsigned long int;


//static Depth max_depth = 0; //
const double NOPATH = 100000000000.0;

//not accessible via std::get<type>
struct Vertex {
  VType      type; 
  Index      index;
  Mem        mem_cap;
  SIMD       simd;
  VID        vid; //global vertex descriptor to make graphs copyable and still reliably refer to the same vertex... overhead due to query, but meh..
  Depth      depth;
};

struct Edge {
  EType      label;
  double     weight;
  EID        eid;
};

using graph_t = boost::adjacency_list<
                     boost::vecS, //  out-edge container selector                  
                     boost::vecS, //  Vector container selector
                     boost::bidirectionalS, //sets bidirectional edge access
                     Vertex,                //vertex property
                     Edge>;                  //edge property (see bundled properties)
// the following settings seem to contradict some internal details of the adjacency list when using out_edges (hypothesis)
                     //boost::no_property,    //graph property 
                     //boost::multisetS>;     //edge container selector... probably the right one.. but might need major changes concerning VD

using ndgraph_t = boost::adjacency_list<boost::vecS,  boost::vecS, boost::undirectedS, Vertex, Edge>;


//graph internally used descriptors for the main graph, probably not derived types, can become invalidated
using ED = typename boost::graph_traits<graph_t>::edge_descriptor;
using VD = typename boost::graph_traits<graph_t>::vertex_descriptor;

graph_t init_graph(const char* t);
graph_t init_graph();

std::string obj_type_toString(hwloc_obj_t & obj);

template<typename G, typename V>
using Distance = std::function<double(V,V,const G&)>;

///enumerating combinatorics
std::vector<std::vector<int>>
comb(const int k, const std::vector<int>& vec);

#endif
