#ifndef INIT_GRAPH_H
#define INIT_GRAPH_H

#include <iostream>
#include <hwloc.h>
#include <boost/graph/adjacency_list.hpp>



struct Vertex {
    std::string name;
    int   	level;
};

struct Edge {
    std::string label;
    double weight; // perhaps you need this later as well, just an example
};

using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Vertex, Edge>; //first for test later DiGraph

graph_t init_graph(const hwloc_topology_t & t);

#endif
