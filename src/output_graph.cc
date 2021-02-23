#include <string>
#include <fstream>
#include <iostream>
#include "../include/init_graph.h"
#include "../include/output_graph.h"
#include <boost/graph/graphviz.hpp>


//####### Printing the graph
class label_writer {
public:

  void operator()(std::ostream& out, const VD& v) const {
    out << "[label=\"" << g[v].type << " #" << g[v].index << "\"]";
  }

  void operator()(std::ostream& out, const ED& e) const {
    out << "[label=\"" << g[e].label << "\"]";
  }
  const graph_t& g;
};

void make_dotfile(const graph_t & g, const std::string& dotf){
  std::ofstream dot(dotf);
  label_writer lw{g};   //constructor declaring what graph to use, to use the label_writer as a functor
  boost::write_graphviz(dot, g, lw, lw);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}

void make_dotfile_nolabel(const graph_t & g, const std::string& dotf){
  std::ofstream dot(dotf);
  boost::write_graphviz(dot, g);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}


//is va a descendent of vb?
bool is_ancestor(const VD& va, const VD& vb, const graph_t& g){

  bool res=false;
  anc_iterator it(g,va);
  VD vcur;
  do{
    vcur=*it;      //update
    if (vcur==vb){ //check
      res=true;    //success
      break;       //exit
    }
  }
  while(*(++it)!=vcur);  //exit if nothing happens (root node is reached)

  return res;
}


//make canonical tree from root to all the elements in group by adding all the vertices along the shortest path with distance, that involves parent/child relationships (the dummy distance for now)
graph_t make_can_tree(const graph_t& s , const VD& gv){
//TODO remove excessive edges and insert vertex and edge properties
  graph_t t; //target graph
  auto range = boost::adjacent_vertices(gv,s);
  //starting vertex is range.first
  VD root_vd = 1;
  std::for_each(range.first, range.second,
      [&](const auto& vd)
      {
        anc_iterator a_it(s,vd);
        while ( *a_it != root_vd){ //==1
          VD curr =*a_it;
          ++a_it; 
          boost::add_edge(curr, *a_it, {"child", 0.0}, t);
          boost::add_edge(*a_it, curr, {"parent", 0.0}, t);
        }
      }
      );
  std::cout << "created hwloc-tree of subgraph." << std::endl;
  return t;
}







//graph_t make_subgraph(const graph_t& s, const VD& gv){
//  //1. copy the whole graph
//  //2. look for all vertices that have no children if they are descendents of members of the group 
//  //if the group is the whole graph then the group will point to the root.
//  //if the group is a subgraph, then an out edge will point towards a vertex lower down in the hierarchy
//  //the subgraph will still include everything up to the root (so it gets included for all groups)
//  //if a subgraph is to exclude members, the branches that are included need to be in the group.
//  //TODO excluding group (inv_group) <- checks if an ancestor is in the excluded group
//  graph_t g(s); 
//    
//  return g;
//}

