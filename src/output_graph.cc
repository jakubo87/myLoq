#include <fstream>
#include <iostream>
#include "../include/init_graph.h"
#include <boost/graph/graphviz.hpp>
//#include <boost/graph/graph_utility.hpp>

void make_dotfile(const graph_t & g){
  const std::string dotf = "out.dot";
  std::ifstream dot("out.dot");
  boost::write_graphviz(dot, g, boost::default_writer());
  std::cout << "Graph has been written to dotfile: " << dotf << std::endl;
}
