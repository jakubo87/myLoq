#include <iostream>
#include <hwloc.h>
#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include <functional>
#include "../include/init_graph.h"
#include "../include/output_graph.h"

///*
//indentation of higher depth opjects
//*/
//std::ostream & operator<<(std::ostream & os,std::pair<std::string,int> p){
//  std::string s;
//  for (int i=0; i<=p.second; ++i){
//    s+=" ";
//  }
//  s+=s+p.first;
//
//  return operator<<(os,s);
//}
//
////general for all types with a std::to_string implementation
//template <class M>
//std::ostream & operator<<(std::ostream & os,std::pair<M,int> p){
//  std::string s;
//  for (int i=0; i<=p.second; ++i){
//    s+=" ";
//  }
//  s+=s+std::to_string(p.first);
//
//  return operator<<(os,s);
//}
//
//
////print topology
//std::ostream & operator<<(std::ostream & os, hwloc_topology_t & t){
//  //std::string s;
//
//  int max_depth=0;
//  int depth=0;
//  std::string s;
//
// //print breadth first
//  os << "breadth first approach\n";
//
//  max_depth = hwloc_topology_get_depth(t);
//
//
//  for (depth = 0; depth < max_depth; depth++) {
//      os << "Objects at level " <<  depth << std::endl;
//      for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
//        auto hw_obj = hwloc_get_obj_by_depth(t, depth, i);
//        s=obj_type_toString(hw_obj) + " " + std::to_string(i) + "\n";
//        s+= "OS_index: " + std::to_string(hw_obj->os_index) + "??? \n";
//        s+= "logical index: " + std::to_string(hw_obj->logical_index);
//        s+='\n';
//        os << std::make_pair(s,depth);
//      }
//    }
//
//  os << "\n";
//
//  return operator<<(os,"");
//}





int main ()
{
  //hwloc_init
  hwloc_topology_t t;
  hwloc_topology_init(&t);  // initialization
  hwloc_topology_set_io_types_filter(t, HWLOC_TYPE_FILTER_KEEP_ALL); //for all the devices, or...
  //hwloc_topology_set_io_types_filter(t,HWLOC_TYPE_FILTER_KEEP_IMPORTANT);
  hwloc_topology_load(t);   // actual detection
  //std::cout << t <<"\n"; //print nodes
  
  auto g = init_graph(t);
//TODO expand graph!
  //cleaning up:
  hwloc_topology_destroy(t); //since data was copied hwloc is not needed anymore
  
  //some testing:
  //make group - at first arbitraty
  //TODO find partitioning
  std::vector<int> vs = {4,5};
  auto i = make_group("Group1", vs, g);
  std::cout << " group 1 has vd: " << i << std::endl; 
  //calc custom distance
  //define a distance function:
  std::function<double(int,int,const graph_t&)> dist1 =  [&](auto va, auto vb, const graph_t& g)
    {
      double res = 130.0;
//      //candidate for querying edges - potentially wasteful to first narrow it down...
//      //or maybe straight up impossible due to graph type limitations..?
//      auto range = boost::edge_range(va, vb, g);
//      //check all edges for label "parent"
//      std::for_each (range.first, range.second,[&](const auto & ei){
//        if (g[ei].label=="parent" && va==source(ei,g) && vb==target(ei,g))
//          res = 0.0;
//      });

      auto range = boost::edges(g);
      //check all edges for label "parent"
      std::for_each (range.first, range.second,[&](const auto & ei){
        if (g[ei].label=="parent" && va==source(ei,g) && vb==target(ei,g))
          res = 0.0;
      });

      return res;
    };
  std::cout << "distance (5,7): " << distance(5,7,g, dist1) << std::endl;
  std::cout << "distance (6,7): " << distance(6,7,g, dist1) << std::endl;

  //auto path = shortest_path(g, 5, 7, dist1); 
  //for (auto i : path){
  //  std::cout << i << " ";
  //} 
  //std::cout << std::endl;

  make_dotfile(g);

  return 0;
}

