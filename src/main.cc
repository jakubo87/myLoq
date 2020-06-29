#include <iostream>
#include <hwloc.h>
#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include "../include/init_graph.h"
#include "../include/output_graph.h"

/*
indentation of higher depth opjects
*/
std::ostream & operator<<(std::ostream & os,std::pair<std::string,int> p){
  std::string s;
  for (int i=0; i<=p.second; ++i){
    s+=" ";
  }
  s+=s+p.first;

  return operator<<(os,s);
}
//general for all types with a std::to_string implementation
template <class M>
std::ostream & operator<<(std::ostream & os,std::pair<M,int> p){
  std::string s;
  for (int i=0; i<=p.second; ++i){
    s+=" ";
  }
  s+=s+std::to_string(p.first);

  return operator<<(os,s);
}



///// somehow not quite working
//void traverse_tree_DF(std::ostream & os, hwloc_obj_t obj, int depth){
//    do{
//      os << std::make_pair(obj_type_toString(obj), depth) << "\n";
//      os << "still alive\n";
//      if (obj->first_child){
//        os << "still alive2\n";
//        traverse_tree_DF(os,obj->memory_first_child,++depth);
//      }
//      os << "still alive3\n";
//      obj=obj->next_sibling;
//    }while (obj);
//    os << "still alive4\n";
//}
//
//void traverse_tree_DF(std::ostream & os, hwloc_obj_t obj){
//  traverse_tree_DF(os,obj, 0);
//}
//


//print topology
std::ostream & operator<<(std::ostream & os, hwloc_topology_t & t){
  //std::string s;

  int max_depth=0;
  int depth=0;
  std::string s;

 //print breadth first
  os << "breadth first approach\n";

  max_depth = hwloc_topology_get_depth(t);


  for (depth = 0; depth < max_depth; depth++) {
      os << "Objects at level " <<  depth << std::endl;
      for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
        auto hw_obj = hwloc_get_obj_by_depth(t, depth, i);
        s=obj_type_toString(hw_obj) + " " + std::to_string(i) + "\n";
        s+= "OS_index: " + std::to_string(hw_obj->os_index) + "??? \n";
        s+= "logical index: " + std::to_string(hw_obj->logical_index);
        s+='\n';
        os << std::make_pair(s,depth);
      }
    }

  os << "\n";

 ////print depth first
 // os << "depth first approach\n";
 // //
 // depth=0;
 // hwloc_obj_t obj=hwloc_get_obj_by_depth(t, 0, depth); //ROOT
 // traverse_tree_DF(os, obj);


  return operator<<(os,"");
}





int main ()
{
  //hwloc_init
  hwloc_topology_t t;
  hwloc_topology_init(&t);  // initialization
  //hwloc_topology_set_io_types_filter(t,HWLOC_TYPE_FILTER_KEEP_IMPORTANT);
  hwloc_topology_load(t);   // actual detection
  //std::cout << t <<"\n"; //print nodes
  
  auto g = init_graph(t);
//TODO expand graph!
  //cleaning up:
  hwloc_topology_destroy(t);
  //since data was copied hwloc is not needed anymore
  //
  make_dotfile(g);
  std::cout << "memory still missing" << std::endl;
  return 0;
}

