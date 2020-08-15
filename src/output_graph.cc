#include <string>
#include <fstream>
#include <iostream>
#include "../include/init_graph.h"
#include <boost/graph/graphviz.hpp>
//#include <boost/graph/graph_utility.hpp>

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
  label_writer lw{g};
  boost::write_graphviz(dot, g, lw, lw); //lw());
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}
