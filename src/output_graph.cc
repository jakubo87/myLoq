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

void make_dotfile(const graph_t & g){
  const std::string dotf = "out.dot";
  std::ofstream dot(dotf);
  label_writer lw{g};   //constructor declaring what graph to use, to use the label_writer as a functor
  boost::write_graphviz(dot, g, lw, lw);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}




//is va a descendent of vb?
bool is_ancestor(const VD& va, const VD& vb, const graph_t& g){

  bool res=false;
  anc_iterator it(g,va);
  VD vcur;
  do{
    vcur=*it;     //update
    if (vcur==vb){   //check
      res=true;    //success
      break;       //exit
    }
  }
  while(*(++it)!=vcur);  //exit if nothing happens (root node is reached)

  return res;
}

//TODO include all canonically connected (parent/child) vertices of included vertices in (group vertex) gv
//// 1. define granularity by picking the depth/level in the group ("want the whole core? want only one PU, pick that!")
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

