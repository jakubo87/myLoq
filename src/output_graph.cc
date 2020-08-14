#include <string>
#include <fstream>
#include <iostream>
#include "../include/init_graph.h"
#include <boost/graph/graphviz.hpp>
//#include <boost/graph/graph_utility.hpp>

class label_writer {
public:
  template <class VertexOrEdge>
  void operator()(std::ostream& out, const VertexOrEdge& v) const {
    out << "[label=\"" << g[v].type << " #" << g[v].index << "\"]";
  }
  const graph_t& g;
};






void make_dotfile(const graph_t & g){
  const std::string dotf = "out.dot";
  std::ofstream dot(dotf);
  label_writer lw{g};
  boost::write_graphviz(dot, g, lw); //lw());
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}
