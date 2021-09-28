#ifndef OUTPUT_GRAPH_H
#define OUTPUT_GRAPH_H

#include "../include/init_graph.h"
#include "../include/hwloc-test.h"
#include "../include/predicates.h"
//####### Printing the graph
template<typename G>
class label_writer{
  using E = typename boost::graph_traits<G>::edge_descriptor;
  using V = typename boost::graph_traits<G>::vertex_descriptor;

public:
  label_writer() = delete; //must have reference to graph
  void operator()(std::ostream& out, const V& v) const {
    out << "[label=\"" << boost::get(&Vertex::type, g, v) << " #" << boost::get(&Vertex::index, g, v) << "\"]";
  }
  void operator()(std::ostream& out, const E& e) const {
    auto label = boost::get(&Edge::label, g, e);
    if (label == "member")    {out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=red]"; return;}
    if (label == "replicates"){out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=blue]"; return;}
    if (label == "partition") {out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=green]"; return;}
    out << "[label=\"" << boost::get(&Edge::label, g, e) << "\", color=black]";
  }
  const G& g;
};

template<typename G>
void make_dotfile(const G& g, const std::string& dotf = "out.dot"){
  std::ofstream dot(dotf);
  label_writer<G> lw{g};   //constructor declaring what graph to use, to use the label_writer as a functor
  boost::write_graphviz(dot, g, lw, lw);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}

template<typename G>
void make_dotfile_nolabel(const G& g, const std::string& dotf = "out.dot"){
  std::ofstream dot(dotf);
  boost::write_graphviz(dot, g);
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}


#endif
