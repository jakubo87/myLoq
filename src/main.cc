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
  


  //###################################################################
  //TESTS:
  
  //BASICS:
  //find vd 
  auto vds = get_vds(
      g,                //the graph
      "HWLOC_OBJ_CORE", //the type
      0);               //the index

  std::cout << "vd of core 0: " << vds[0] << std::endl;

  //generic vd query TODO does this also work when queries are generated at runtime...???
  vds = test_get_vds(g, VType("HWLOC_OBJ_CORE"));
  std::cout << "testing generic querying for vds... CORES have vd: " << std::endl;
  for (auto& v : vds)
    std::cout << v << " ";
  std::cout << std::endl;
  std::cout << "VDs with rubbish type: " << std::endl;
  vds = test_get_vds(g, VType("xxx"));
  for (auto& v : vds)
    std::cout << v << " ";
  std::cout << std::endl;
  //  vds = test_get_vds(g, 1); <- does not compile you have to write the explicit type
  std::cout << "searching for VD with type: CORE and index: 1 (testing maching in reverse order to what it is in the vertex struct): " << std::endl;
  vds = test_get_vds(g, Index(1), VType("HWLOC_OBJ_CORE"));
  for (auto& v : vds)
    std::cout << v << " ";
  std::cout << std::endl;


  
  //find edge properties
  std::cout << "ed label from vd 1 to 2: " << get_edge_label(g, get_ed(g,1,2,"parent").at(0)) << std::endl;
  //find edges by their property
  std::cout << "List all edges with the parent property:" << std::endl;
  auto eds = get_eds(g, "parent");
  for (const auto& ed : eds){
    std::cout << boost::source(ed,g) << " to " << boost::target(ed,g) << std::endl;
  }


  //GROUPS
  //make group - at first arbitrary
  std::vector<VD> vs = {10,12};
  auto i = make_group("Group1", vs, g);
  std::cout << " Group1 has vd: " << i << std::endl; 

  //return the group members of group 
  std::cout << "The members of Group1 are the following:" << std::endl;
  //TODO make path queries like "is connected to group", or "is 'child' of cache" 


  //DISTANCES
  //calc custom distance
  //define a distance function:
  std::function<double(VD,VD,const graph_t&)> dist1 =  [&](auto va, auto vb, const graph_t& g)
    {
      //the result, if the graph has no direct edge in any allowed category defined by this function
      double res = NOPATH;  //default 
      auto range = boost::edges(g);
      //check all edges for label "child"
       std::for_each (range.first, range.second,[&](const auto & ei){
        if (g[ei].label=="child" && va==source(ei,g) && vb==target(ei,g))
          res = 10.0;       //case of rising in the hierarchy
      });
      //check all edges for label "parent"
      std::for_each (range.first, range.second,[&](const auto & ei){
        if (g[ei].label=="parent" && va==source(ei,g) && vb==target(ei,g))
          res = 0.0;        //case when descending in hierarchy
      });

      return res;
    };


  std::cout << "distance (5,7): " << distance(5,7,g, dist1) << std::endl;
  std::cout << "distance (6,7): " << distance(6,7,g, dist1) << std::endl;

  std::cout << "distance (8,9): " << distance(8,9,g, dist1) << std::endl;
  std::cout << "path from 9 to 8:" << std::endl; 
  shortest_path(g, 8, 9, dist1); 

  //this returns the shortest distance found over multiple hops - or a direct edge - with respect to only a given distance function
  std::cout << "combined distances from 8 to:" << std::endl;
  for(long unsigned int i=0; i < boost::num_vertices(g); ++i){
    std::cout << i << ": " ;
    std::cout << find_distance(g,8,i,dist1) << std::endl;
  }
    

  //TODO find partitioning

  //ancestry iterator
  std::cout << "is 3 an ancestor of 11?: " << is_ancestor(11,3,g) << std::endl;
  std::cout << "is 11 an ancestor of 3?: " << is_ancestor(3,11,g) << std::endl;



  //TODO return subgraph
  make_dotfile_nolabel(g,"totalnl.dot");
  make_dotfile(g,"total.dot");

  //auto g2 = make_tree(g,1);
  //make_dotfile(g2,"hwloc.dot");

  return 0;
}

